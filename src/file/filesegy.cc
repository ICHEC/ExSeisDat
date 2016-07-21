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
#include "global.hh"
#include "file/filesegy.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
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

/*! \brief Convert a 2 byte \c char array in big endian to a host short
 *  \return Return a short
 */
template <typename T = short>
T getHostShort(const uchar * src)
{
    return (T(src[0]) << 8) | T(src[1]);
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

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
SEGY::SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
           const std::shared_ptr<Obj::Interface> obj_) : File::Interface(piol_, name_, obj_)
{
   Init();
}

SEGY::SEGY(const std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt,
           const Obj::Opt & objOpt, const Data::Opt & dataOpt) : Interface(piol_, name_, objOpt, dataOpt)
{
    Init();
}
#pragma GCC diagnostic pop

///////////////////////////////////       Member functions      ///////////////////////////////////
void SEGY::Init()
{
    size_t hoSz = SEGSz::getHOSz();
    size_t fsz = obj->getFileSz();
    if (fsz >= hoSz)
    {
        std::vector<uchar> buf(hoSz);
        obj->readHO(buf.data());
        parseHO(buf.data(), fsz);
    }
    else
    {
        ns = 0U;
        nt = 0U;
        text = "";
    }
}

void SEGY::parseHO(uchar * buf, const size_t fsz)
{
    ns = getMd(Hdr::NumSample, buf);
    nt = (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz(ns);

    getAscii(piol, name, buf, SEGSz::getTextSz());
    for (size_t i = 0; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}
}}
