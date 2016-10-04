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
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "share/segy.hh"
#include "file/iconv.hh"
#include "share/units.hh"
#include "share/datatype.hh"

namespace PIOL { namespace File {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*! Header offsets as defined in the specification. Actual offset is the value minus one.
 */
enum class Hdr : size_t
{
    Increment  = 3217U, //!< int16_t. The increment between traces in microseconds
    NumSample  = 3221U, //!< int16_t. The number of samples per trace
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
    TORF        = 13U   //!< int32_t. The trace number within the ORF.
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
    xSrc = 73U,  //!< int32_t. The X coordinate for the source
    ySrc = 77U,  //!< int32_t. The Y coordinate for the source
    xRcv = 81U,  //!< int32_t. The X coordinate for the receive group
    yRcv = 85U,  //!< int32_t. The Y coordinate for the receive group
    xCMP = 181U, //!< int32_t  The X coordinate for the CMP
    yCMP = 185U  //!< int32_t. The Y coordinate for the CMP
};

/*! Trace Header offsets to grid components
 */
enum class TrGrd : size_t
{
    il        = 189U, //!< int32_t. The Inline grid point.
    xl        = 193U  //!< int32_t. The Crossline grid point.
};

/*! \brief Return a pair of coordinate offsets
 *  \param[in] pair The coordinate point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#endif
std::pair<TrCrd, TrCrd> getPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(TrCrd::xSrc, TrCrd::ySrc);
        case Coord::Rcv :
            return std::make_pair(TrCrd::xRcv, TrCrd::yRcv);
        default :
        case Coord::CMP :
            return std::make_pair(TrCrd::xCMP, TrCrd::yCMP);
    }
}

/*! \brief Return a pair of grid offsets
 *  \param[in] pair The grid point type to return
 *  \return The pair of offset enums
 */
#ifndef __ICC
constexpr
#endif
std::pair<TrGrd, TrGrd> getPair(Grid pair)
{
    switch (pair)
    {
//Note: When a new set of grid points are required:
//        case Grid::OFR :
//            return std::make_pair(TrHdr::ORF, TrHdr::TORF);
        default :
        case Grid::Line :
            return std::make_pair(TrGrd::il, TrGrd::xl);
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

/*! \brief Convert a SEG-Y scale integer to a floating point type
 *  \param[in] scale The int16_t scale taken from the SEG-Y file
 *  \return The scale convertered to floating point.
 */
geom_t scaleConv(int16_t scale)
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
geom_t getMd(const TrScal scal, const uchar * src)
{
    auto scale = getHost<int16_t>(&src[size_t(scal)-1U]);
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
 *  \param[in] src The metadata value to insert into the buffer.
 *  \param[in, out] dst The header as an array of uchar.
 */
template <typename T = int16_t>
void setMd(const Hdr item, const T src, uchar * dst)
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

/*! \brief Set a trace scale in the trace header
 *  \param[in] item The scale item of interest
 *  \param[in] scale The metadata value to insert into the buffer.
 *  \param[in, out] buf The trace header as an array of uchar.
 */
void setScale(const TrScal item, const int16_t scale, uchar * buf)
{
    getBigEndian(scale, &buf[size_t(item)-1U]);
}

/*! \brief Set a coordinate point in the trace header
 *  \param[in] item The coordinate point type of interest
 *  \param[in] coord The value of the coordinate point
 *  \param[in] scale The scale as an integer from the SEG-Y header
 *  \param[in, out] buf The trace header as an array of uchar.
 */
void setCoord(const Coord item, const coord_t coord, const int16_t scale, uchar * buf)
{
    auto pair = getPair(item);
    geom_t gscale = scaleConv(scale);
    getBigEndian(int32_t(std::lround(coord.x / gscale)), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(std::lround(coord.y / gscale)), &buf[size_t(pair.second) - 1U]);
}

//TODO: unit test
/*! \brief Get a coordinate point from the trace header
 *  \param[in] item The coordinate point type of interest
 *  \param[in] scale The scale from the SEG-Y header
 *  \param[in] buf A buffer containing the trace header
 *  \param[in, out] buf The trace header as an array of uchar.
 *  \return The coordinate point associated with the coordinate item
 */
coord_t getCoord(const Coord item, const geom_t scale, const uchar * buf)
{
    auto p = getPair(item);
    return coord_t(getMd(p.first, scale, buf),
                   getMd(p.second, scale, buf));
}

/*! \brief Get a grid point from the trace header
 *  \param[in] item The grid type of interest
 *  \param[in] buf A buffer containing the trace header
 *  \param[in, out] buf The trace header as an array of uchar.
 *  \return The grid point associated with the requested grid
 */
grid_t getGrid(const Grid item, const uchar * buf)
{
    auto p = getPair(item);
    return grid_t(getMd(p.first, buf),
                  getMd(p.second, buf));
}

/*! \brief Set a grid point in the trace header
 *  \param[in] item The grid point type of interest
 *  \param[in] grid The value of the grid point
 *  \param[out] buf The trace header as an array of uchar.
 */
void setGrid(const Grid item, const grid_t grid, uchar * buf)
{
    auto pair = getPair(item);
    getBigEndian(int32_t(grid.il), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(grid.xl), &buf[size_t(pair.second) - 1U]);
}

/*! \fn int16_t PIOL::File::deScale(const geom_t val)
 * \brief Take a coordinate and extract a suitable scale factor to represent that number
 * in 6 byte fixed point format of the SEG-Y specification.
 * \param[in] val The coordinate of interest.
 * \return An appropriate scale factor for the coordinate.
 * \details Convert the number from float to a 6 byte SEGY fixed-point representation.
 * There are ten possible values for the scale factor. Shown are the possible values
 * and the form the input float should have to use that scale factor.
 * firstly, anything smaller than 4 decimal points is discarded since the approach
 * can not represent it.
*//*
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
    llint llintpart = llint(val);
    int32_t intpart = llintpart;
    if (llintpart != intpart)
    {
        /* Starting with the smallest scale factor, see
        *  what is the smallest scale we can apply and still
        *  hold the integer portion.
        *  We drop as much precision as it takes to store
        *  the most significant digit. */
        for (int32_t scal = 10; scal <= tenk ; scal *= 10)
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

/*! Compare two scales and return the appropriate one which maximises precision
 *  while preventing overflow of the int32_t type.
 *  \param[in] scal1 The first scale value
 *  \param[in] scal2 The second scale value
 *  \return The scal value which meets the precision criteria.
 */
int16_t scalComp(int16_t scal1, int16_t scal2)
{
    //if the scale is bigger than 1 that means we need to use the largest
    //to ensure conservation of the most significant digit
    //otherwise we choose the scale that preserves the most digits
    //after the decimal place.
    if (scal1 > 1 || scal2 > 1)
        return std::max(scal1, scal2);
    else
        return std::min(scal1, scal2);
}

/*! Extract the scale value from each coordinate from a coordinate point
 *  and return the most appropriate scale value that maximises precision
 *  while preventing overflow of the int32_t type.
 *  \param[in] coord A coordinate point
 *  \return The extracted scal value which meets the precision criteria.
 */
int16_t calcScale(const coord_t coord)
{
    //I get the minimum value so that I definitely store the result.
    //This is at the expense of precision.
    int16_t scal1 = deScale(coord.x);
    int16_t scal2 = deScale(coord.y);
    return scalComp(scal1, scal2);
}

void extractTraceParam(const uchar * md, TraceParam * prm)
{
    geom_t scale = getMd(TrScal::ScaleCoord, md);
    prm->src = getCoord(Coord::Src, scale, md);
    prm->rcv = getCoord(Coord::Rcv, scale, md);
    prm->cmp = getCoord(Coord::CMP, scale, md);
    prm->line = getGrid(Grid::Line, md);
    prm->tn = getHost<int32_t>(&md[size_t(TrHdr::SeqFNum)-1]);
}

void insertTraceParam(const TraceParam * prm, uchar * md)
{
    int16_t scale = scalComp(1, calcScale(prm->src));
    scale = scalComp(scale, calcScale(prm->rcv));
    scale = scalComp(scale, calcScale(prm->cmp));
    setScale(TrScal::ScaleCoord, scale, md);

    setCoord(Coord::Src, prm->src, scale, md);
    setCoord(Coord::Rcv, prm->rcv, scale, md);
    setCoord(Coord::CMP, prm->cmp, scale, md);
    setGrid(Grid::Line, prm->line, md);

    //narrowing conversion of tn
    getBigEndian(int32_t(prm->tn), &md[size_t(TrHdr::SeqFNum) - 1U]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
SEGY::Opt::Opt(void)
{
    incFactor = SI::Micro;
}

SEGY::SEGY(const Piol piol_, const std::string name_, const File::SEGY::Opt & opt, std::shared_ptr<Obj::Interface> obj_, const FileMode mode_)
    : Interface(piol_, name_, obj_)
{
    Init(opt, mode_);
}

SEGY::SEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_, const FileMode mode)
    : Interface(piol_, name_, obj_)
{
    File::SEGY::Opt opt;
    Init(opt, mode);
}

SEGY::~SEGY(void)
{
    if (!piol->log->isErr())
    {
        if (state.resize)
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
        if (mode != FileMode::Read && state.writeHO && !piol->comm->getRank())
        {
            std::vector<uchar> buf(SEGSz::getHOSz());
            packHeader(buf.data());
            obj->writeHO(buf.data());
        }
    }
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void SEGY::packHeader(uchar * buf) const
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

void SEGY::procHeader(csize_t fsz, uchar * buf)
{
    obj->readHO(buf);
    ns = getMd(Hdr::NumSample, buf);
    nt = SEGSz::getNt(fsz, ns);
    inc = geom_t(getMd(Hdr::Increment, buf)) * incFactor;
    format = static_cast<Format>(getMd(Hdr::Type, buf));

    getAscii(piol.get(), name, SEGSz::getTextSz(), buf);
    for (size_t i = 0U; i < SEGSz::getTextSz(); i++)
        text.push_back(buf[i]);
}

void SEGY::Init(const File::SEGY::Opt & segyOpt, const FileMode mode_)
{
    mode = mode_;
    incFactor = segyOpt.incFactor;
    memset(&state, 0, sizeof(Flags));
    size_t hoSz = SEGSz::getHOSz();
    size_t fsz = obj->getFileSz();
    if (fsz >= hoSz && mode != FileMode::Write)
    {
        auto buf = std::make_unique<uchar[]>(hoSz);
        procHeader(fsz, buf.get());
    }
    else
    {
        format = Format::IEEE;
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

void SEGY::writeNs(csize_t ns_)
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

void SEGY::writeNt(csize_t nt_)
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
}

void SEGY::writeInc(const geom_t inc_)
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

void SEGY::readTrace(csize_t offset, csize_t sz, trace_t * trace) const
{
    if (sz == 0 || ns == 0 || offset > nt)   //Nothing to be read
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "readTrace() was called for a zero byte read.", Log::Verb::None);
        return;
    }
    obj->readDODF(offset, ns, sz, reinterpret_cast<uchar *>(trace));

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

void SEGY::writeTrace(csize_t offset, csize_t sz, trace_t * trace)
{
    #ifdef NT_LIMITS
    if (sz+offset > NT_LIMITS)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "writeTrace() was called with an implied write of an nt value that is too large", Log::Verb::None);
        return;
    }
    #endif

    if (sz == 0 || ns == 0)   //Nothing to be written.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "writeTrace() was called for a zero byte write. ns: "
            + std::to_string(ns) + " sz: " + std::to_string(sz), Log::Verb::None);
        return;
    }
    uchar * buf = reinterpret_cast<uchar *>(trace);

    //TODO: Check cache effects doing both of these loops the other way.
    //TODO: Add unit test for reverse4Bytes
    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]); //TODO: Add length check

    obj->writeDODF(offset, ns, sz, buf);

    for (size_t i = 0; i < ns * sz; i++)
        reverse4Bytes(&buf[i*sizeof(float)]);

    if (offset + sz >= nt)
    {
        if (state.resize == true)
            state.resize = false;
        if (offset + sz > nt)
            nt = offset + sz;
    }
}

//TODO: Unit test
void SEGY::readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const
{
    if (sz == 0 || offset >= nt)   //Nothing to be read.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "readTraceParam() was called for a zero byte read", Log::Verb::None);
        return;
    }
    //Don't process beyond end of file
    size_t nsz = (offset + sz > nt ? offset - nt : sz);

    std::vector<uchar> buf(SEGSz::getMDSz() * nsz);

    obj->readDOMD(offset, ns, nsz, buf.data());

    for (size_t i = 0; i < nsz; i++)
        extractTraceParam(&buf[i * SEGSz::getMDSz()], &prm[i]);
}

void SEGY::writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm)
{
    #ifdef NT_LIMITS
    if (sz+offset > NT_LIMITS)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "writeTraceParam() was called with an implied write of an nt value that is too large", Log::Verb::None);
        return;
    }
    #endif
    if (sz == 0)   //Nothing to be written.
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
            "writeTraceParam() was called for a zero byte write", Log::Verb::None);
        return;
    }
    std::vector<uchar> buf(SEGSz::getMDSz() * sz);

    for (size_t i = 0; i < sz; i++)
        insertTraceParam(&prm[i], &buf[i * SEGSz::getMDSz()]);

    obj->writeDOMD(offset, ns, sz, buf.data());

    if (offset + sz > nt)
    {
        nt = offset + sz;

        //There is probably a mismatch between desired nt size and actual
        state.resize = true;
    }
}
}}
