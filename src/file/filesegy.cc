#include <vector>
#include "global.hh"
#include "file/filesegy.hh"
#include "share/segy.hh"

#include <iostream>
namespace PIOL { namespace File {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class Hdr : size_t
{
    Increment = 3217, //Short
    NumSample = 3221, //Short
    Type = 3225 //Short, Trace data type. AKA format in SEGY terminology
};

template <typename T = short>
T getHostShort(const uchar * src)
{
    return (T(src[0]) << 8) | T(src[1]);
}

template <class T=int>
T getMd(Hdr val, const uchar * buf)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        return getHostShort(&buf[size_t(val)-1]);
        default :
        return 0;
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
    }
}

void SEGY::parseHO(const uchar * buf, const size_t fsz)
{
    ns = getMd(Hdr::NumSample, buf);
    nt = (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz(ns);
}
size_t SEGY::readNs(void)
{
    return ns;
}
size_t SEGY::readNt(void)
{
    return nt;
}

}}

