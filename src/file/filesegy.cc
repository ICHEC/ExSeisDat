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
#include <limits>
#include <iostream>
namespace PIOL { namespace File {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment  = 3217U, //!< int16_t. The increment between traces in microseconds
    NumSample  = 3221U, //!< int16_t. The Numbe of samples per trace
    Type       = 3225U, //!< int16_t. Trace data type. AKA format in SEGY terminology
    Sort       = 3229U, //!< int16_t. The sort order of the traces.
    Units      = 3255U, //!< int16_t. The unit system, i.e SI or imperial.
    SEGYFormat = 3501U, //!< int16_t. The SEG-Y Revision number
    FixedTrace = 3503U, //!< int16_t. Whether we are using fixed traces or not.
    Extensions = 3505U, //!< int16_t. If we use header extensions or not.
};
/*! Misc Trace Header offsets
 */
enum class TrHdr : size_t
{
    SeqNum      = 1U,   //!< int32_t. The trace sequence number within the Line
    SeqFNum     = 5U,   //!< int32_t. The trace sequence number within SEG-Y File
    ORF         = 9U,   //!< int32_t. The original field record number.
    TORF        = 9U    //!< int32_t. The trace number within the ORF.
};

/*! Trace Header offsets to elevations
 */
enum class TrElev : size_t
{
    RcvElv      = 41U,  //!< int32_t. The Receiver group elevation
    SurfElvSrc  = 45U,  //!< int32_t. The surface elevation at the source.
    SrcDpthSurf = 49U,  //!< int32_t. The source depth below surface (opposite of above?).
    DtmElvRcv   = 53U,  //!< int32_t. The datum elevation for the receiver group.
    DtmElvSrc   = 57U,  //!< int32_t. The datum elevation for the source.
    WtrDepSrc   = 61U,  //!< int32_t. The water depth for the source.
    WtrDepRcv   = 65U   //!< int32_t. The water depth for the receive group.
};

/*! Trace Header offsets to scalers
 */
enum class TrScal : size_t
{
    ScaleElev   = 69U,  //!< int16_t. The scale coordinate for 41-68 (elevations + depths)
    ScaleCoord  = 71U   //!< int16_t. The scale coordinate for 73-88 + 181-188
};

/*! Trace Header offsets to coordinates
 */
enum class TrCrd : size_t
{
    xSrc        = 73U,  //!< int32_t. The X coordinate for the source
    ySrc        = 77U,  //!< int32_t. The Y coordinate for the source
    xRcv        = 81U,  //!< int32_t. The X coordinate for the receive group
    yRcv        = 85U,  //!< int32_t. The Y coordinate for the receive group
    xCDP        = 181U, //!< int32_t  The X coordinate for the CDP
    yCDP        = 185U  //!< int32_t. The Y coordinate for the CDP
};

/*! Trace Header offsets to grid components
 */
enum class TrGrd : size_t
{
    iLin        = 189U, //!< int32_t. The Inline grid point.
    xLin        = 193U  //!< int32_t. The Crossline grid point.
};

/*! \brief Return a pair of coordinate offsets
 *  \param[in] pair The coordinate point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
std::pair<TrCrd, TrCrd> getPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(TrCrd::xSrc, TrCrd::ySrc);
        case Coord::Rcv :
            return std::make_pair(TrCrd::xRcv, TrCrd::yRcv);
        case Coord::Cmp :
            return std::make_pair(TrCrd::xCDP, TrCrd::yCDP);
    }
}

/*! \brief Return a pair of grid offsets
 *  \param[in] pair The grid point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
std::pair<TrGrd, TrGrd> getPair(Grid pair)
{
    switch (pair)
    {
//Note: When a new set of grid points are required:
//        case Grid::OFR :
//            return std::make_pair(TrHdr::ORF, TrHdr::TORF);
        case Grid::Line :
            return std::make_pair(TrGrd::iLin, TrGrd::xLin);
    }
}

/*! \brief Get the header metadata value from the binary header.
 *  \param[in] item The header item of interest.
 *  \param[in] src The buffer of the header object.
 *  \return Return the header item value.
 */
template <class T = int16_t>
T getMd(const Hdr item, const uchar * src)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        return T(getHost<int16_t>(&src[size_t(item)-1U]));
        default :
        return T(0);
        break;
    }
}

geom_t scaleConv(int32_t scale)
{
    scale = (!scale ? 1 : scale);
    return (scale > 0 ? geom_t(scale) : geom_t(1)/geom_t(-scale));
}

/*! \brief Get the specified scale multipler from the Trace header.
 *  \param[in] scal The scalar of interest.
 *  \param[in] src The buffer of the header object.
 *  \return Return the scalar value.
 *
 *  \details If the integer value on disk is negative, the inverse
 *  of the absolute value is returned. If the value is zero,
 *  1 is returned, otherwise the value is returned. No check is done
 *  to ensure other restrictions are in place (i.e 1, 10, 1000 etc).
 */
geom_t getMd(const TrScal scal, uchar * src)
{
    int32_t scale = getHost<int32_t>(&src[size_t(scal)-1U]);
    return scaleConv(scale);
}

/*! \brief Get the specified coordinate from the Trace header.
 *  \param[in] item The specific coordinate to get
 *  \param[in] scale The scale factor
 *  \param[in] src The buffer of the header object.
 *  \return Return the coordinate
 */
geom_t getMd(const TrCrd item, const geom_t scale, const uchar * src)
{
    return scale * geom_t(getHost<int32_t>(&src[size_t(item)-1U]));
}

/*! \brief Get the specified grid component from the Trace header.
 *  \param[in] item The specific grid component to get.
 *  \param[in] src The buffer of the header object.
 *  \return Return the grid component (two components make a grid point).
 */
int32_t getMd(const TrGrd item, const uchar * src)
{
    return getHost<int32_t>(&src[size_t(item)-1U]);
}

/*! \brief Set the header metadata value corresponding to the item specified
 *  \param[in] item The header item of interest
 *  \param[in] dst The header as an array of uchar.
 *  \param[in] src The metadata value to insert into the buffer.
 *  \return Return the header item value
 */
template <typename T = int16_t>
void setMd(Hdr item, uchar * dst, T src)
{
    switch (item)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        case Hdr::Units :
        case Hdr::SEGYFormat :
        case Hdr::FixedTrace :
        case Hdr::Extensions :
        getBigEndian<int16_t>(src, &dst[static_cast<size_t>(item)-1U]);
        default :
        break;
    }
}

void setScale(TrScal item, int16_t scale, uchar * buf)
{
    getBigEndian(scale, &buf[size_t(item)-1U]);
    std::cout << "scale = " << scale << std::endl;
}

void setCoord(const Coord item, const coord_t coord, const int16_t scale, uchar * buf)
{
    auto pair = getPair(item);
    geom_t gscale = scaleConv(scale);
    getBigEndian(int32_t(std::lround(coord.first / gscale)), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(std::lround(coord.second / gscale)), &buf[size_t(pair.second) - 1U]);
    int32_t f = std::lround(coord.first / gscale);
    int32_t s = std::lround(coord.second / gscale);
    std::cout << "Final Values = " << f << std::endl;
    std::cout << "Final Values = " << s << std::endl;
}

void setGrid(const Grid item, const grid_t grid, uchar * buf)
{
    auto pair = getPair(item);
    getBigEndian(int32_t(grid.first), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(grid.second), &buf[size_t(pair.second) - 1U]);
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
    if (!piol->log->isErr())
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
}

///////////////////////////////////       Member functions      ///////////////////////////////////
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

void SEGY::writeText(const std::string text_)
{
    if (text != text_)
    {
        text = text_;
        text.resize(SEGSz::getTextSz());
        state.writeHO = true;
    }
}

void SEGY::writeNs(const size_t ns_)
{
    if (ns_ > size_t(std::numeric_limits<int16_t>::max()))
    {
        piol->record(name, Log::Layer::File, Log::Status::Error, "Ns value is too large for SEG-Y", Log::Verb::None);
        return;
    }

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

void SEGY::writeNt(const size_t nt_)
{
#ifdef NT_LIMITS
    if (nt_ > NT_LIMITS)
    {
        const std::string msg = "nt_ beyond limited size: "  + std::to_string(NT_LIMITS) + " in writeNt()";
        piol->record(name, Log::Layer::File, Log::Status::Error, msg, Log::Verb::None);
    }
#endif

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

void SEGY::writeInc(const geom_t inc_)
{
    if (!std::isnormal(inc_))
    {
        piol->record(name, Log::Layer::File, Log::Status::Error,
            "The SEG-Y Increment " + std::to_string(inc_) + "is not normal.", Log::Verb::None);
        return;
    }

    if (inc != inc_)
    {
        inc = inc_;
        state.writeHO = true;
    }
}

coord_t SEGY::readCoordPoint(const Coord item, const size_t i)
{
    std::vector<uchar> buf(SEGSz::getMDSz()); //Small.
    uchar * md = buf.data();
    obj->readDOMD(i, ns, md);

    geom_t scale = getMd(TrScal::ScaleCoord, md);

    auto pair = getPair(item);
    return coord_t(getMd(pair.first, scale, md),
                   getMd(pair.second, scale, md));
}

grid_t SEGY::readGridPoint(const Grid item, const size_t i)
{
    std::vector<uchar> buf(SEGSz::getMDSz()); //Small.
    uchar * md = buf.data();
    obj->readDOMD(i, ns, md);

    auto pair = getPair(item);
    return std::make_pair(getMd(pair.first, md), getMd(pair.second, md));
}

/* Convert the number from float to a 6 byte SEGY fixed-point representation.
 * There are ten possible values for the scale factor. Shown are the possible values
 * and the form the input float should have to use that scale factor.
 * firstly, anything smaller than 4 decimal points is discarded since the approach
 * can not represent it.
 * Shown is the
 * position of the least significant digit:
 * -10000 - \d0000.0000
 * -1000  - \d000.0000
 * -100   - \d00.0000
 * -10    - \d0.0000
 * -1     - \d
 * 1      - \d
 * 10     - \d.\d
 * 100    - \d.\d\d
 * 1000   - \d.\d\d\d
 * 10000  - \d.\d\d\d\d
 * TODO: Handle the annoying case of numbers at or around 2147483648 with a decimal somewhere.
 * TODO: Add rounding before positive scale values
*/
int16_t deScale(const geom_t val)
{
    constexpr llint tenk = 10000;
    //First we need to determine what scale is required to store the
    //biggest decimal value of the int.
    llint llintpart = llint(val);      //TODO: Double check spec
    int32_t intpart = llintpart;          //TODO: Double check spec
    if (llintpart != intpart)
    {
        /* Starting with the smallest scale factor, see
        *  what is the smallest scale we can apply and still
        *  hold the integer portion.
        *  We drop as much precision as it takes to store
        *  the most significant digit. */
        for (int32_t scal = 1; scal <= tenk ; scal *= 10)
        {
            llint v = llintpart / scal;
            int32_t iv = v;
            if (v == iv)
                return scal;
        }
        return 0;
    }
    else
    {
        //Get the first four digits
        llint digits = std::llround(val*geom_t(tenk)) - llintpart*tenk;
        //if the digits are all zero we don't need any scaling
        if (digits != 0)
        {
            //We try the most negative scale values we can first. (scale = - 10000 / i)
            for (int32_t i = 1; i < tenk ; i *= 10)
            {
                if (digits % (i*10))
                {
                    int16_t scaleFactor = -tenk / i;
                    //Now we test that we can still store the most significant byte
                    geom_t scal = scaleConv(scaleFactor);

                    //int32_t t = llint(val / scal) - digits;
                    int32_t t = std::lround(val / scal);
                    t /= -scaleFactor;

                    if (t == llintpart)
                        return scaleFactor;
                }
            }
        }
        return 1;
    }
}
void SEGY::writeCoordPoint(const Coord item, const size_t i, coord_t coord)
{
    std::vector<uchar> md(SEGSz::getMDSz()); //Small.

    //I get the minimum value so that I definitely store the result.
    //This is at the expense of precision.
    int16_t scale;
    int16_t scal1 = deScale(coord.first);
    int16_t scal2 = deScale(coord.second);

    //if the scale is bigger than 1 that means we need to use the largest
    //to ensure conservation of the most significant digit
    //otherwise we choose the scale that preserves the most digits
    //after the decimal place.
    if (scal1 > 1 || scal2 > 1)
        scale = std::max(scal1, scal2);
    else
        scale = std::min(scal1, scal2);

    setScale(TrScal::ScaleCoord, scale, md.data());
    setCoord(item, coord, scale, md.data());
    obj->writeDOMD(i, ns, md.data());
}

void SEGY::writeGridPoint(const Grid item, const size_t i, const grid_t grid)
{
    std::vector<uchar> md(SEGSz::getMDSz()); //Small.
    setGrid(item, grid, md.data());
    obj->writeDOMD(i, ns, md.data());
}
}}
