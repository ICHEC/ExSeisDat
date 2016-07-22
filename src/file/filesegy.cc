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
#include <type_traits>
#include <cmath>
#include "global.hh"
#include "file/filesegy.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
#include "share/units.hh"
namespace PIOL { namespace File {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment = 3217U,   //!< Short
    NumSample = 3221U,   //!< Short
    Type = 3225U         //!< Short, Trace data type. AKA format in SEGY terminology
};

enum Format : int16_t
{
    IBM = 1,    //IBM format, big endian
    TC4,    //Two's complement, 4 byte
    TC2,    //Two's complement, 2 byte
    FPG,    //Fixed-point gain (obsolete)
    IEEE,
    NA1,
    NA2,
    TC1    //Two's complement, 1 byte
};


/*! \brief Convert a 2 byte \c char array in big endian to a host short
 *  \return Return a short
 */
template <typename T = int16_t>
T getHostShort(const uchar * src)
{
    return (T(src[0]) << 8) | T(src[1]);
}

/*! \brief Convert a 4 byte \c char array in big endian to a host 4 byte type
 *  \param[in] src The input 4 byte type with host endianness
 *  \param[out] dst A pointer to 4 bytes of data where the big endian
 *  short is stored.
 */
//template <typename T,
//typename std::enable_if<sizeof(T) == 4U, T>::type = 0 >
template <typename T, typename std::enable_if<sizeof(T) == 4U, T>::type = 0>
void getBigEndian(uchar * dst, T src)
{
    assert(sizeof(T) == 4);
    dst[0] = src >> 24 & 0xFF;
    dst[1] = src >> 16 & 0xFF;
    dst[2] = src >> 8 & 0xFF;
    dst[3] = src & 0xFF;
}

/*! \brief Convert a 2 byte \c char array in big endian to a host 2 byte type
 *  \param[in] src The input short with host endianness
 *  \param[out] dst A pointer to 2 bytes of data where the big endian
 *  short is stored.
 */
template <typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
void getBigEndian(const T src, uchar * dst)
{
    dst[0] = src >> 8 & 0xFF;
    dst[1] = src & 0xFF;
}

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
        return getHostShort(&buf[size_t(val)-1]);
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
        auto buf = std::make_unique<uchar[]>(SEGSz::getHOSz());
        packHeader(buf.get());
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
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
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
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
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

    setMd(Hdr::NumSample, buf, static_cast<int16_t>(ns));
    setMd(Hdr::Type, buf, static_cast<int16_t>(Format::IEEE));
    setMd(Hdr::Increment, buf, static_cast<int16_t>(std::lround(inc / incFactor)));
    obj->writeHO(buf);
}

void SEGY::procHeader(const size_t fsz, uchar * buf)
{
    obj->readHO(buf);
    ns = getMd(Hdr::NumSample, buf);
    nt = (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz(ns);
    inc = getMd(Hdr::Increment, buf) * incFactor;

    getAscii(piol, name, buf, SEGSz::getTextSz());
    for (size_t i = 0U; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}

void SEGY::Init(const File::SEGYOpt & segyOpt)
{
    incFactor = segyOpt.incFactor;
    memset(&state, 1, sizeof(Flags));
    state.writeHO = true;
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
    }
}
}}
