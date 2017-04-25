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

void ReadSEGY::readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm, csize_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    if (trace == TRACE_NULL)
    {
        std::vector<uchar> buf(SEGSz::getMDSz() * sz);
        obj->readDOMD(offset, ns, ntz, buf.data());
        extractParam(ntz, buf.data(), prm, 0U, skip);
    }
    else
    {
        uchar * tbuf = reinterpret_cast<uchar *>(trace);
        if (prm == PARAM_NULL)
            obj->readDODF(offset, ns, ntz, tbuf);
        else
        {
            std::vector<uchar> dobuf(ntz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
            obj->readDO(offset, ns, ntz, dobuf.data());

            for (size_t i = 0; i < sz; i++)
                std::copy(&dobuf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &dobuf[(i+1) * SEGSz::getDOSz(ns)],
                            &tbuf[i * SEGSz::getDFSz(ns)]);

            extractParam(ntz, dobuf.data(), prm, SEGSz::getDFSz(ns), skip);
        }

        if (trace != NULL)
        {
            if (format == Format::IBM)
                for (size_t i = 0; i < ns * ntz; i ++)
                    trace[i] = convertIBMtoIEEE(trace[i], true);
            else
                for (size_t i = 0; i < ns * ntz; i++)
                    reverse4Bytes(&tbuf[i*sizeof(float)]);
        }
    }
}

void ReadSEGY::readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm, csize_t skip) const
{
    if (trace == TRACE_NULL)
    {
        std::vector<uchar> buf(SEGSz::getMDSz() * sz);
        obj->readDOMD(ns, sz, offset, buf.data());
        extractParam(sz, buf.data(), prm, 0U, skip);
    }
    else
    {
        uchar * tbuf = reinterpret_cast<uchar *>(trace);
        if (prm == PARAM_NULL)
            obj->readDODF(ns, sz, offset, tbuf);
        else
        {
            std::vector<uchar> dobuf(sz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
            obj->readDO(ns, sz, offset, dobuf.data());
            extractParam(sz, dobuf.data(), prm, SEGSz::getDFSz(ns), skip);

            for (size_t i = 0; i < sz; i++)
                std::copy(&dobuf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &dobuf[(i+1) * SEGSz::getDOSz(ns)],
                          &tbuf[i * SEGSz::getDFSz(ns)]);
        }

        if (trace != NULL)
        {
            if (format == Format::IBM)
                for (size_t i = 0; i < ns * sz; i ++)
                    trace[i] = convertIBMtoIEEE(trace[i], true);
            else
            {
                uchar * buf = reinterpret_cast<uchar *>(trace);
                for (size_t i = 0; i < ns * sz; i++)
                    reverse4Bytes(&buf[i*sizeof(float)]);
            }
        }
    }
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

    File::Param sprm(prm->r, nodups.size());
    std::vector<trace_t> strace(ns * nodups.size());

    readTrace(nodups.size(), nodups.data(), strace.data(), &sprm, 0U);

    size_t n = 0;
    for (size_t j = 0; j < sz; j++)
    {
        n += (j && offset[idx[j-1]] != offset[idx[j]]);
        cpyPrm(n, &sprm, skip + idx[j], prm);
        for (size_t k = 0; k < ns; k++)
            trace[idx[j]*ns + k] = strace[n*ns + k];
    }
}
}}
