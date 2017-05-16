/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details ReadSEGY functions
 *//*******************************************************************************************/
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "file/iconv.hh"
#include "share/misc.hh"
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
ReadSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, const ReadSEGY::Opt & opt, std::shared_ptr<Obj::Interface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    Init(opt);
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    ReadSEGY::Opt opt;
    Init(opt);
}
///////////////////////////////////       Member functions      ///////////////////////////////////
void ReadSEGY::procHeader(size_t fsz, uchar * buf)
{
    ns = getMd(Hdr::NumSample, buf);
    nt = SEGSz::getNt(fsz, ns);
    inc = geom_t(getMd(Hdr::Increment, buf)) * incFactor;
    format = static_cast<Format>(getMd(Hdr::Type, buf));

    getAscii(piol.get(), name, SEGSz::getTextSz(), buf);
    for (size_t i = 0U; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}

void ReadSEGY::Init(const ReadSEGY::Opt & opt)
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
        ns = 0U;
        nt = 0U;
        inc = geom_t(0);
        text = "";
    }
}

size_t ReadSEGY::readNt(void)
{
    return nt;
}

void ReadSEGY::procTrace(size_t sz, std::function<size_t(size_t)> offset, const std::vector<uchar> & buf,
                         trace_t * trc, Param * prm, csize_t skip) const
{
    uchar * tbuf = reinterpret_cast<uchar *>(trc);
    if (prm != PARAM_NULL)
    {
        extractParam(sz, buf.data(), prm, (trc != TRACE_NULL ? SEGSz::getDFSz(ns) : 0U), skip);
        for (size_t i = 0; i < sz; i++)
            setPrm(i, Meta::ltn, offset(i), prm);
    }

    if (trc != NULL && trc != TRACE_NULL)
    {
        if (format == Format::IBM)
            for (size_t i = 0; i < ns * sz; i ++)
                trc[i] = convertIBMtoIEEE(trc[i], true);
        else
            for (size_t i = 0; i < ns * sz; i++)
                reverse4Bytes(&tbuf[i*sizeof(float)]);
    }
}

template <typename T>
void readTraceT(Obj::Interface * obj, const Format format, csize_t ns, const T offset, std::function<size_t(size_t)> offunc,
                                      csize_t sz, trace_t * trc, Param * prm, csize_t skip)
{
    uchar * tbuf = reinterpret_cast<uchar *>(trc);
    if (prm == PARAM_NULL)
        obj->readDODF(offset, ns, sz, tbuf);
    else
    {
        csize_t blockSz = (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
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

        extractParam(sz, buf, prm, (trc != TRACE_NULL ? SEGSz::getDFSz(ns) : 0U), skip);
        for (size_t i = 0; i < sz; i++)
            setPrm(i, Meta::ltn, offunc(i), prm);
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


void ReadSEGY::readTrace(csize_t offset, csize_t sz, trace_t * trc, Param * prm, csize_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    if (offset >= nt && sz)   //Nothing to be read.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "readParam() was called for a zero byte read", Log::Verb::None);
        return;
    }
    readTraceT(obj.get(), format, ns, offset, [offset] (size_t i) -> size_t { return offset + i; }, ntz, trc, prm, skip);
}

void ReadSEGY::readTrace(csize_t sz, csize_t * offset, trace_t * trc, Param * prm, csize_t skip) const
{
    readTraceT(obj.get(), format, ns, offset,  [offset] (size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}

void ReadSEGY::readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trc, Param * prm, csize_t skip) const
{
    //Sort the initial offset and make a new offset without duplicates
    auto idx = getSortIndex(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++)
        if (offset[idx[j-1]] != offset[idx[j]])
            nodups.push_back(offset[idx[j]]);

    File::Param sprm(prm->r, (prm != PARAM_NULL ? nodups.size() : 0U));
    std::vector<trace_t> strc(ns * (trc != TRACE_NULL ? nodups.size() : 0U));

    readTrace(nodups.size(), nodups.data(), (trc != TRACE_NULL ? strc.data() : trc),
                                            (prm != PARAM_NULL ? &sprm : prm), 0U);

    if (prm != PARAM_NULL)
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
