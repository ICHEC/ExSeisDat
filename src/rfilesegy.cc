/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date July 2016
 *   @brief
 *   @details ReadSEGY functions
 *//*******************************************************************************************/
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "file/characterconversion.hh"
#include "share/misc.hh"
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
ReadSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

ReadSEGY::ReadSEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const ReadSEGY::Opt & opt, std::shared_ptr<Obj::Interface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    incFactor = opt.incFactor;
    size_t hoSz = SEGSz::getHOSz();
    size_t fsz = obj->getFileSz();

    if (fsz >= hoSz)
    {
        auto buf = std::vector<uchar>(hoSz);
        obj->readHO(buf.data());
        procHeader(fsz, buf.data());
    }
    else
    {
        format = Format::IEEE;
        ns = 0LU;
        nt = 0LU;
        inc = geom_t(0);
        text = "";
    }
}

ReadSEGY::ReadSEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_)
    : ReadSEGY(piol_, name_, ReadSEGY::Opt(), obj_)
{
}
///////////////////////////////////       Member functions      ///////////////////////////////////

ReadSEGYModel::ReadSEGYModel(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_) : ReadSEGYModel(piol_, name_, ReadSEGYModel::Opt(), obj_)
{
}

ReadSEGYModel::ReadSEGYModel(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const ReadSEGYModel::Opt& opt, std::shared_ptr<Obj::Interface> obj_) : ReadSEGY(piol_, name_, opt, obj_)
{
    std::vector<size_t> vlist = {0LU, 1LU, ReadSEGY::readNt() - 1LU};
    File::Param prm(vlist.size());
    readParamNonContiguous(vlist.size(), vlist.data(), &prm);

    llint il0 = File::getPrm<llint>(0LU, PIOL_META_il, &prm);
    llint xl0 = File::getPrm<llint>(0LU, PIOL_META_xl, &prm);

    llint ilInc = File::getPrm<llint>(1LU, PIOL_META_il, &prm) - il0;
    llint ilNum = (ilInc ? (File::getPrm<llint>(2LU, PIOL_META_il, &prm) - il0) / ilInc : 0LU);
    llint xlNum = (ReadSEGY::readNt() / (ilNum ? ilNum : 1LU));
    llint xlInc = (File::getPrm<llint>(2LU, PIOL_META_xl, &prm) - xl0) / xlNum;

    ilInc = (ilInc ? ilInc : 1LU);
    xlInc = (xlInc ? xlInc : 1LU);

    il = std::make_tuple(il0, ilNum, ilInc);
    xl = std::make_tuple(xl0, xlNum, xlInc);
}

std::vector<trace_t> ReadSEGYModel::readModel(const size_t offset, const size_t sz, const Uniray<size_t, llint, llint> & gather)
{
    std::vector<trace_t> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++)
    {
        auto val = gather[offset + i];
        /* The below can be translated to:
         * trace number = ilNumber * xlInc + xlNumber
         * much like indexing in a 2d array.
         */
        offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il)) * std::get<1>(xl)
                  + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
    }

    readTraceNonContiguous(
        offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);
    return trc;
}

std::vector<trace_t> ReadSEGYModel::readModel(const size_t sz, const size_t * offset, const Uniray<size_t, llint, llint> & gather)
{
    std::vector<trace_t> trc(sz * readNs());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++)
    {
        auto val = gather[offset[i]];
        offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il)) * std::get<1>(xl)
                  + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
    }

    readTraceNonContiguous(
        offsets.size(), offsets.data(), trc.data(), PIOL_PARAM_NULL, 0LU);
    return trc;
}

void ReadSEGY::procHeader(size_t fsz, uchar * buf)
{
    ns = getMd(Hdr::NumSample, buf);
    nt = SEGSz::getNt(fsz, ns);
    inc = geom_t(getMd(Hdr::Increment, buf)) * incFactor;
    format = static_cast<Format>(getMd(Hdr::Type, buf));

    getAscii(piol.get(), name, SEGSz::getTextSz(), buf);
    for (size_t i = 0LU; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}

size_t ReadSEGY::readNt(void) const
{
    return nt;
}

/*! Template function for reading SEG-Y traces and parameters, random and contiguous.
 *  @tparam T The type of offset (pointer or size_t)
 *  @param[in] obj The object-layer object.
 *  @param[in] format The format of the trace data.
 *  @param[in] ns The number of samples per trace.
 *  @param[in] offset The offset(s). If T == size_t * this is an array, otherwise its a single offset.
 *  @param[in] offunc A function which given the ith trace of the local process, returns the associated trace offset.
 *  @param[in] sz The number of traces to read
 *  @param[in] trc Pointer to trace array.
 *  @param[in] prm Pointer to parameter structure.
 *  @param[in] skip Skip \c skip entries in the parameter structure
 */
template <typename T>
void readTraceT(Obj::Interface * obj, const Format format, const size_t ns, const T offset, std::function<size_t(size_t)> offunc,
                                      const size_t sz, trace_t * trc, Param * prm, const size_t skip)
{
    uchar * tbuf = reinterpret_cast<uchar *>(trc);
    if (prm == PIOL_PARAM_NULL)
        obj->readDODF(offset, ns, sz, tbuf);
    else
    {
        const size_t blockSz = (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> alloc(blockSz * sz);
        uchar * buf = (sz ? alloc.data() : nullptr);

        if (trc == TRACE_NULL)
            obj->readDOMD(offset, ns, sz, buf);
        else
        {
            obj->readDO(offset, ns, sz, buf);
            for (size_t i = 0; i < sz; i++)
                std::copy(&buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &buf[(i+1) * SEGSz::getDOSz(ns)],
                            &tbuf[i * SEGSz::getDFSz(ns)]);
        }

        extractParam(sz, buf, prm, (trc != TRACE_NULL ? SEGSz::getDFSz(ns) : 0LU), skip);
        for (size_t i = 0; i < sz; i++)
            setPrm(i+skip, PIOL_META_ltn, offunc(i), prm);
    }

    if (trc != TRACE_NULL && trc != nullptr)
    {
        if (format == Format::IBM)
            for (size_t i = 0; i < ns * sz; i ++)
                trc[i] = convertIBMtoIEEE(trc[i], true);
        else
            for (size_t i = 0; i < ns * sz; i++)
                reverse4Bytes(&tbuf[i*sizeof(float)]);
    }
}

void ReadSEGY::readTrace(const size_t offset, const size_t sz, trace_t * trc, Param * prm, const size_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    if (offset >= nt && sz)   //Nothing to be read.
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
                          "readParam() was called for a zero byte read", PIOL_VERBOSITY_NONE);
    readTraceT(obj.get(), format, ns, offset, [offset] (size_t i) -> size_t { return offset + i; }, ntz, trc, prm, skip);
}

void ReadSEGY::readTraceNonContiguous(const size_t sz, const size_t * offset, trace_t * trc, Param * prm, const size_t skip) const
{
    readTraceT(obj.get(), format, ns, offset,  [offset] (size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}

void ReadSEGY::readTraceNonMonotonic(const size_t sz, const size_t * offset, trace_t * trc, Param * prm, const size_t skip) const
{
    //Sort the initial offset and make a new offset without duplicates
    auto idx = getSortIndex(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++)
        if (offset[idx[j-1]] != offset[idx[j]])
            nodups.push_back(offset[idx[j]]);

    File::Param sprm(prm->r, (prm != PIOL_PARAM_NULL ? nodups.size() : 0LU));
    std::vector<trace_t> strc(ns * (trc != TRACE_NULL ? nodups.size() : 0LU));

    readTraceNonContiguous(
        nodups.size(), nodups.data(),
        (trc != TRACE_NULL ? strc.data() : trc),
        (prm != PIOL_PARAM_NULL ? &sprm : prm), 0LU);

    if (prm != PIOL_PARAM_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j)
        {
            n += (j && offset[idx[j-1]] != offset[idx[j]]);
            cpyPrm(n, &sprm, skip + idx[j], prm);
        }

    if (trc != TRACE_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j)
        {
            n += (j && offset[idx[j-1]] != offset[idx[j]]);
            for (size_t k = 0; k < ns; k++)
                trc[idx[j]*ns + k] = strc[n*ns + k];
        }
}
}}
