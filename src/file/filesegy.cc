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
#include "global.hh"
#include "file/filesegy.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
#include "share/units.hh"
#include "share/datatype.hh"
#include <iostream>
namespace PIOL { namespace File {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment  = 3217U, //!< Short. The increment between traces in microseconds
    NumSample  = 3221U, //!< Short. The Numbe of samples per trace
    Type       = 3225U, //!< Short. Trace data type. AKA format in SEGY terminology
    Sort       = 3229U, //!< Short. The sort order of the traces.
    Units      = 3255U, //!< Short. The unit system, i.e SI or imperial.
    SEGYFormat = 3501U, //!< Short. The SEG-Y Revision number
    FixedTrace = 3503U, //!< Short. Whether we are using fixed traces or not.
    Extensions = 3505U, //!< Short. If we use header extensions or not.
};

/*! \brief Get the header metadata value corresponding to the item specified
 *  \param[in] val The header item of interest
 *  \param[in] buf The buffer of the header object
 *  \return Return the header item value
 */
template <class T = int>
T getMd(const Hdr val, const uchar * buf)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        return T(getHostShort(&buf[static_cast<size_t>(val)-1U]));
        default :
        return T(0);
        break;
    }
}

/*! \brief Get the header metadata value corresponding to the item specified
 *  \param[in] val The header item of interest
 *  \param[in] buf The buffer of the header object
 *  \return Return the header item value
 */
template <typename T = int16_t>
void setMd(Hdr val, uchar * dst, T src)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        getBigEndian<int16_t>(src, &dst[static_cast<size_t>(val)-1U]);
        default :
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
SEGYOpt::SEGYOpt(void)
{
    incFactor = SI::Micro;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
SEGY::SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
           const std::shared_ptr<Obj::Interface> obj_) : File::Interface(piol_, name_, obj_)
{
   Init(segyOpt);
}

SEGY::SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
           const Obj::Opt & objOpt, const Data::Opt & dataOpt) : Interface(piol_, name_, objOpt, dataOpt)
{
    Init(segyOpt);
}
#pragma GCC diagnostic pop

SEGY::~SEGY(void)
{
    if (state.resize)
        obj->setFileSz(SEGSz::getFileSz(nt, ns));

    if (state.writeHO)
    {
        std::vector<uchar> buf(SEGSz::getHOSz());
        packHeader(buf.data());
        obj->writeHO(buf.data());
    }
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void SEGY::writeText(std::string text_)
{
    if (text != text_)
    {
        text = text_;
        state.writeHO = true;
    }
}

void SEGY::writeNs(size_t ns_)
{
    if (ns != ns_)
    {
        ns = ns_;
        if (nt != 0U)           // If nt is zero this operaton is pointless.
        {
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
            state.resize = false;
        }
        else
            state.resize = true;

        state.writeHO = true;
    }
}

void SEGY::writeNt(size_t nt_)
{
    if (nt != nt_)
    {
        nt = nt_;
        if (ns != 0U)       // If ns is zero this operaton is pointless
        {
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
            state.resize = false;
        }
        else
            state.resize = true;
    }
}

void SEGY::writeInc(geom_t inc_)
{
    if (inc != inc_)
    {
        inc = inc_;
        state.writeHO = true;
    }
}

void SEGY::packHeader(uchar * buf)
{
    for (size_t i = 0; i < text.size(); i++)
        buf[i] = text[i];

    setMd(Hdr::NumSample, buf, int16_t(ns));
    setMd(Hdr::Type,      buf, int16_t(Format::IEEE));
    setMd(Hdr::Increment, buf, int16_t(std::lround(inc / incFactor)));

//Currently these are hard-coded entries:
    setMd(Hdr::Units,      buf, 0x0001);    //The unit system.
    setMd(Hdr::SEGYFormat, buf, 0x0100);    //The version of the SEGY format.
    setMd(Hdr::FixedTrace, buf, 0x0001);    //We always deal with fixed traces at present.
    setMd(Hdr::Extensions, buf, 0x0000);    //We do not support text extensions at present.
}

void SEGY::procHeader(const size_t fsz, uchar * buf)
{
    obj->readHO(buf);
    ns = getMd(Hdr::NumSample, buf);
    nt = (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz(ns);
    inc = geom_t(getMd(Hdr::Increment, buf)) * incFactor;
    format = static_cast<Format>(getMd(Hdr::Type, buf));

    getAscii(piol, name, buf, SEGSz::getTextSz());
    for (size_t i = 0U; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}

void SEGY::Init(const File::SEGYOpt & segyOpt)
{
    incFactor = segyOpt.incFactor;
    memset(&state, 0, sizeof(Flags));
    size_t hoSz = SEGSz::getHOSz();
    size_t fsz = obj->getFileSz();
    if (fsz >= hoSz)
    {
        auto buf = std::make_unique<uchar[]>(hoSz);
        procHeader(fsz, buf.get());
    }
    else
    {
        ns = 0U;
        nt = 0U;
        inc = geom_t(0);
        text = "";
        state.writeHO = true;
    }
}
}}
