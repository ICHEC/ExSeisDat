/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <cstring>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include <iterator>
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
#include "share/units.hh"
#include "share/datatype.hh"
#include "file/segymd.hh"
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
ReadSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

WriteSEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, const ReadSEGY::Opt & opt, std::shared_ptr<Obj::Interface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    Init(opt);
}

WriteSEGY::WriteSEGY(const Piol piol_, const std::string name_, const WriteSEGY::Opt & opt, std::shared_ptr<Obj::Interface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
    Init(opt);
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    ReadSEGY::Opt opt;
    Init(opt);
}

WriteSEGY::WriteSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
    WriteSEGY::Opt opt;
    Init(opt);
}

WriteSEGY::~WriteSEGY(void)
{
    if (!piol->log->isErr())
    {
        calcNt();
        if (state.resize)
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
        if (state.writeHO)
        {
            if (!piol->comm->getRank())
            {
                std::vector<uchar> buf(SEGSz::getHOSz());
                packHeader(buf.data());
                obj->writeHO(buf.data());
            }
            else
                obj->writeHO(NULL);
        }
    }
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void WriteSEGY::packHeader(uchar * buf) const
{
    for (size_t i = 0; i < text.size(); i++)
        buf[i] = text[i];

    setMd(Hdr::NumSample, int16_t(ns), buf);
    setMd(Hdr::Type, int16_t(Format::IEEE), buf);
    setMd(Hdr::Increment, int16_t(std::lround(inc / incFactor)), buf);

//Currently these are hard-coded entries:
    setMd(Hdr::Units,      0x0001, buf);    //The unit system.
    setMd(Hdr::SEGYFormat, 0x0100, buf);    //The version of the SEGY format.
    setMd(Hdr::FixedTrace, 0x0001, buf);    //We always deal with fixed traces at present.
    setMd(Hdr::Extensions, 0x0000, buf);    //We do not support text extensions at present.
}

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

void WriteSEGY::Init(const WriteSEGY::Opt & opt)
{
    incFactor = opt.incFactor;
    memset(&state, 0, sizeof(Flags));
    format = Format::IEEE;
    ns = 0U;
    nt = 0U;
    inc = geom_t(0);
    text = "";
    state.writeHO = true;
}

size_t ReadSEGY::readNt(void)
{
    return nt;
}

size_t WriteSEGY::calcNt(void)
{
    if (state.stalent)
    {
        nt = piol->comm->max(nt);
        state.stalent = false;
        state.resize = true;
    }
    return nt;
}

void WriteSEGY::writeText(const std::string text_)
{
    if (text != text_)
    {
        text = text_;
        text.resize(SEGSz::getTextSz());
        state.writeHO = true;
    }
}

void WriteSEGY::writeNs(csize_t ns_)
{
    if (ns_ > size_t(std::numeric_limits<int16_t>::max()))
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error, "Ns value is too large for SEG-Y", Log::Verb::None);
        return;
    }

    if (ns != ns_)
    {
        ns = ns_;
        state.resize = true;
        state.writeHO = true;
    }
}

void WriteSEGY::writeNt(csize_t nt_)
{
#ifdef NT_LIMITS
    if (nt_ > NT_LIMITS)
    {
        const std::string msg = "nt_ beyond limited size: "  + std::to_string(NT_LIMITS) + " in writeNt()";
        piol->log->record(name, Log::Layer::File, Log::Status::Error, msg, Log::Verb::None);
    }
#endif

    if (nt != nt_)
    {
        nt = nt_;
        state.resize = true;
    }
    state.stalent = false;
}

void WriteSEGY::writeInc(const geom_t inc_)
{
    if (std::isnormal(inc_) == false)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "The SEG-Y Increment " + std::to_string(inc_) + " is not normal.", Log::Verb::None);
        return;
    }

    if (inc != inc_)
    {
        inc = inc_;
        state.writeHO = true;
    }
}

void ReadSEGY::readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm, csize_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));
    uchar * buf = reinterpret_cast<uchar *>(trace);

    if (prm == PARAM_NULL)
        obj->readDODF(offset, ns, ntz, buf);
    else
    {
        std::vector<uchar> dobuf(ntz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
        obj->readDO(offset, ns, ntz, dobuf.data());

        if (ntz)
            extractParam(ntz, dobuf.data(), prm, SEGSz::getDFSz(ns), skip);

        for (size_t i = 0; i < ntz; i++)
            std::copy(&dobuf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &dobuf[(i+1) * SEGSz::getDOSz(ns)],
                      buf + i * SEGSz::getDFSz(ns));
    }
    if (format == Format::IBM)
        for (size_t i = 0; i < ns * ntz; i ++)
            trace[i] = convertIBMtoIEEE(trace[i], true);
    else
        for (size_t i = 0; i < ns * ntz; i++)
            reverse4Bytes(&buf[i*sizeof(float)]);
}

void WriteSEGY::writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm, csize_t skip)
{
    #ifdef NT_LIMITS
    if (sz+offset > NT_LIMITS)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "writeTrace() was called with an implied write of an nt value that is too large", Log::Verb::None);
        return;
    }
    #endif

    uchar * buf = reinterpret_cast<uchar *>(trace);

    //TODO: Check cache effects doing both of these loops the other way.
    //TODO: Add unit test for reverse4Bytes
    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]); //TODO: Add length check

    if (prm == PARAM_NULL)
        obj->writeDODF(offset, ns, sz, buf);
    else
    {
        std::vector<uchar> dobuf(sz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
        if (sz)
            insertParam(sz, prm, dobuf.data(), SEGSz::getDFSz(ns), skip);
        for (size_t i = 0; i < sz; i++)
            std::copy(&buf[i * SEGSz::getDFSz(ns)], &buf[(i+1) * SEGSz::getDFSz(ns)],
                      dobuf.begin() + i * SEGSz::getDOSz(ns) + SEGSz::getMDSz());
        obj->writeDO(offset, ns, sz, dobuf.data());
    }

    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]);

    state.stalent = true;
    nt = std::max(offset + sz, nt);
}

//TODO: Unit test
void ReadSEGY::readParam(csize_t offset, csize_t sz, Param * prm, csize_t skip) const
{
    if (offset >= nt && sz)   //Nothing to be read.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "readParam() was called for a zero byte read", Log::Verb::None);
        return;
    }

    //Don't process beyond end of file if we can
    size_t ntz = (!sz ? sz : (offset + sz > nt ? nt - offset : sz));

    std::vector<uchar> buf(SEGSz::getMDSz() * ntz);
    obj->readDOMD(offset, ns, ntz, buf.data());

    if (prm != nullptr && ntz)
        extractParam(ntz, buf.data(), prm, 0, skip);
}

void WriteSEGY::writeParam(csize_t offset, csize_t sz, const Param * prm, csize_t skip)
{
    #ifdef NT_LIMITS
    if (sz+offset > NT_LIMITS)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "writeParam() was called with an implied write of an nt value that is too large", Log::Verb::None);
        return;
    }
    #endif
    if (!sz)   //Nothing to be written.
    {
        obj->writeDOMD(0, 0, size_t(0), nullptr);
        return;
    }
    std::vector<uchar> buf(SEGSz::getMDSz() * sz);

    if (prm != nullptr)
        insertParam(sz, prm, buf.data(), 0U, skip);

    obj->writeDOMD(offset, ns, sz, buf.data());

    state.stalent = true;
    nt = std::max(offset + sz, nt);
}

void ReadSEGY::readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm, csize_t skip) const
{
    uchar * buf = reinterpret_cast<uchar *>(trace);
    if (prm == PARAM_NULL)
        obj->readDODF(ns, sz, offset, buf);
    else
    {
        std::vector<uchar> dobuf(sz * SEGSz::getDOSz(ns)); //FIXME: Potentially a big allocation
        obj->readDO(ns, sz, offset, dobuf.data());

        if (sz)
            extractParam(sz, dobuf.data(), prm, SEGSz::getDFSz(ns), skip);

        for (size_t i = 0; i < sz; i++)
            std::copy(&dobuf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &dobuf[(i+1) * SEGSz::getDOSz(ns)],
                      buf + i * SEGSz::getDFSz(ns));
    }

    if (format == Format::IBM)
        for (size_t i = 0; i < ns * sz; i ++)
            trace[i] = convertIBMtoIEEE(trace[i], true);
    else
        for (size_t i = 0; i < ns * sz; i++)
            reverse4Bytes(&buf[i*sizeof(float)]);
}

void WriteSEGY::writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm, csize_t skip)
{
    uchar * buf = reinterpret_cast<uchar *>(trace);

    //TODO: Check cache effects doing both of these loops the other way.
    //TODO: Add unit test for reverse4Bytes
    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]); //TODO: Add length check

    if (prm == PARAM_NULL)
        obj->writeDODF(ns, sz, offset, buf);
    else
    {
        std::vector<uchar> dobuf(sz * SEGSz::getDOSz(ns));          //FIXME: Potentially a big allocation
        if (sz)
            insertParam(sz, prm, dobuf.data(), SEGSz::getDFSz(ns), skip);
        for (size_t i = 0; i < sz; i++)
            std::copy(&buf[i * SEGSz::getDFSz(ns)], &buf[(i+1) * SEGSz::getDFSz(ns)],
                      dobuf.begin() + i * SEGSz::getDOSz(ns) + SEGSz::getMDSz());
        obj->writeDO(ns, sz, offset, dobuf.data());
    }

    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]);

    state.stalent = true;
    if (sz)
        nt = std::max(offset[sz-1]+1U, nt);
}

void ReadSEGY::readParam(csize_t sz, csize_t * offset, Param * prm, csize_t skip) const
{
//TODO: Is it useful to check if all the offsets are greater than nt?
    if (!sz)   //Nothing to be written.
        obj->readDOMD(0, 0, nullptr, nullptr);
    else
    {
        std::vector<uchar> buf(SEGSz::getMDSz() * sz);
        obj->readDOMD(ns, sz, offset, buf.data());
        if (prm != nullptr)
            extractParam(sz, buf.data(), prm, 0U, skip);
    }
}

void WriteSEGY::writeParam(csize_t sz, csize_t * offset, const Param * prm, csize_t skip)
{
    #ifdef NT_LIMITS
    size_t max = 0;
    for (size_t i = 0; i < sz; i++)
        max = std::max(offset[i], max);

    if (sz+max > NT_LIMITS)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "writeParam() was called with an implied write of an nt value that is too large", Log::Verb::None);
        return;
    }
    #endif

    if (!sz)   //Nothing to be written.
    {
        obj->writeDOMD(0, 0, nullptr, nullptr);
        return;
    }
    std::vector<uchar> buf(SEGSz::getMDSz() * sz);

    if (prm != nullptr)
        insertParam(sz, prm, buf.data(), 0U, skip);

    obj->writeDOMD(ns, sz, offset, buf.data());

    state.stalent = true;
    nt = std::max(offset[sz-1]+1U, nt);
}
}}
