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
                         trace_t * trace, Param * prm, csize_t skip) const
{
    uchar * tbuf = reinterpret_cast<uchar *>(trace);
    if (prm != PARAM_NULL)
    {
        extractParam(sz, buf.data(), prm, (trace != TRACE_NULL ? SEGSz::getDFSz(ns) : 0U), skip);
        for (size_t i = 0; i < sz; i++)
            setPrm(i, Meta::ltn, offset(i), prm);
    }

    if (trace != NULL && trace != TRACE_NULL)
    {
        if (format == Format::IBM)
            for (size_t i = 0; i < ns * sz; i ++)
                trace[i] = convertIBMtoIEEE(trace[i], true);
        else
            for (size_t i = 0; i < ns * sz; i++)
                reverse4Bytes(&tbuf[i*sizeof(float)]);
    }
}


void ReadSEGY::readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm, csize_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    std::vector<uchar> buf;
    uchar * tbuf = reinterpret_cast<uchar *>(trace);
    if (trace == TRACE_NULL)
    {
        buf.resize(SEGSz::getMDSz() * ntz);
        obj->readDOMD(offset, ns, ntz, buf.data());
    }
    else if (prm == PARAM_NULL)
        obj->readDODF(offset, ns, ntz, tbuf);
    else
    {
        buf.resize(ntz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
        obj->readDO(offset, ns, ntz, buf.data());

        for (size_t i = 0; i < ntz; i++)
            std::copy(&buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &buf[(i+1) * SEGSz::getDOSz(ns)],
                        &tbuf[i * SEGSz::getDFSz(ns)]);
    }
    procTrace(ntz, [offset] (size_t i) -> size_t { return offset + i; }, buf, trace, prm, skip);
}

void ReadSEGY::readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm, csize_t skip) const
{
    uchar * tbuf = reinterpret_cast<uchar *>(trace);
    std::vector<uchar> buf;
    if (trace == TRACE_NULL)
    {
        buf.resize(SEGSz::getMDSz() * sz);
        obj->readDOMD(ns, sz, offset, buf.data());
    }
    else if (prm == PARAM_NULL)
        obj->readDODF(ns, sz, offset, tbuf);
    else
    {
        buf.resize(sz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
        obj->readDO(ns, sz, offset, buf.data());

        for (size_t i = 0; i < sz; i++)
            std::copy(&buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &buf[(i+1) * SEGSz::getDOSz(ns)],
                      &tbuf[i * SEGSz::getDFSz(ns)]);
    }

    procTrace(sz, [offset] (size_t i) -> size_t { return offset[i]; }, buf, trace, prm, skip);
}

void ReadSEGY::readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trace, Param * prm, csize_t skip) const
{
    //Sort the initial offset and make a new offset without duplicates
    auto idx = getSortIndex(sz, offset);
    std::vector<size_t> nodups;
    nodups.push_back(offset[idx[0]]);
    for (size_t j = 1; j < sz; j++)
        if (offset[idx[j-1]] != offset[idx[j]])
            nodups.push_back(offset[idx[j]]);

    File::Param sprm(prm->r, (prm != PARAM_NULL ? nodups.size() : 0U));
    std::vector<trace_t> strace(ns * (trace != TRACE_NULL ? nodups.size() : 0U));

    readTrace(nodups.size(), nodups.data(), (trace != TRACE_NULL ? strace.data() : trace),
                                            (prm != PARAM_NULL ? &sprm : prm), 0U);

    if (prm != PARAM_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j, n += offset[idx[j-1]] != offset[idx[j]])
            cpyPrm(n, &sprm, skip + idx[j], prm);

    if (trace != TRACE_NULL)
        for (size_t n = 0, j = 0; j < sz; ++j, n += offset[idx[j-1]] != offset[idx[j]])
            for (size_t k = 0; k < ns; k++)
                trace[idx[j]*ns + k] = strace[n*ns + k];
}
}}
