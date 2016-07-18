#include <vector>
#include "global.hh"
#include "file/filesegy.hh"
#include "share/segy.hh"

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
SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_)
           : File::Interface(piol_, name_, obj_)
{
   Init();
}

SEGY::SEGY(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const File::SEGYOpt & segyOpt, const Obj::Opt & objOpt,
                                              const Data::Opt & dataOpt) : Interface(piol_, name_, objOpt, dataOpt)
{
    Init();
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void SEGY::Init()
{
    size_t hoSz = SEGSz::getHOSz();
    if (obj->getFileSz() >= hoSz)
    {
        std::vector<uchar> buf(hoSz);
        obj->readHO(buf.data());
        parseHO(buf.data());
    }
    else
    {
        ns = 0U;
        nt = 0U;
    }
}

void SEGY::parseHO(const uchar * buf)
{
    ns = getMd(Hdr::NumSample, buf);
    size_t fsz = obj->getFileSz();
    nt = (fsz - SEGSz::getHOSz()) / SEGSz::getDOSz(ns);

    //TODO: Move to unit test
//    assert(ns != 0);
//    assert(fsz - nt*Obj::SEGSz::getDOSz<float>(ns) - Obj::SEGSz::getHOSz() == 0);
//    assert(fsz - obj->getSize(nt, ns) == 0);
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

