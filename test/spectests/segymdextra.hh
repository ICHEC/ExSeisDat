/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date October 2016
 *   @brief
 *   @details
 *//*******************************************************************************************/
#ifndef PIOLFILETESTSEGYMD_INCLUDE_GUARD
#define PIOLFILETESTSEGYMD_INCLUDE_GUARD

#include "file/characterconversion.hh"
#include "file/filesegy.hh"
#include "global.hh"
#include "object/object.hh"
#include "share/datatype.hh"
#include "share/segy.hh"
#include "share/units.hh"

#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

namespace PIOL {
namespace File {

/*! Misc Trace Header offsets
 */
enum class TrHdr : size_t {
    SeqNum  = 1U,  //!< int32_t. The trace sequence number within the Line
    SeqFNum = 5U,  //!< int32_t. The trace sequence number within SEG-Y File
    ORF     = 9U,  //!< int32_t. The original field record number.
    TORF    = 13U  //!< int32_t. The trace number within the ORF.
};

/*! Trace Header offsets to elevations
 */
enum class TrElev : size_t {
    RcvElv     = 41U,  //!< int32_t. The Receiver group elevation
    SurfElvSrc = 45U,  //!< int32_t. The surface elevation at the source.
    SrcDpthSurf =
      49U,  //!< int32_t. The source depth below surface (opposite of above?).
    DtmElvRcv = 53U,  //!< int32_t. The datum elevation for the receiver group.
    DtmElvSrc = 57U,  //!< int32_t. The datum elevation for the source.
    WtrDepSrc = 61U,  //!< int32_t. The water depth for the source.
    WtrDepRcv = 65U   //!< int32_t. The water depth for the receive group.
};

/*! Trace Header offsets to scalers
 */
enum class TrScal : size_t {
    ScaleElev =
      69U,  //!< int16_t. The scale coordinate for 41-68 (elevations + depths)
    ScaleCoord = 71U  //!< int16_t. The scale coordinate for 73-88 + 181-188
};

/*! Trace Header offsets to coordinates
 */
enum class TrCrd : size_t {
    xSrc = 73U,   //!< int32_t. The X coordinate for the source
    ySrc = 77U,   //!< int32_t. The Y coordinate for the source
    xRcv = 81U,   //!< int32_t. The X coordinate for the receive group
    yRcv = 85U,   //!< int32_t. The Y coordinate for the receive group
    xCMP = 181U,  //!< int32_t  The X coordinate for the CMP
    yCMP = 185U   //!< int32_t. The Y coordinate for the CMP
};

/*! Trace Header offsets to grid components
 */
enum class TrGrd : size_t {
    il = 189U,  //!< int32_t. The Inline grid point.
    xl = 193U   //!< int32_t. The Crossline grid point.
};

struct coord_t {
    geom_t x;  //!< The x coordinate
    geom_t y;  //!< The y coordinate

    /*! Constructor for initialising coordinates
     *  @param[in] x_ intialisation value for x
     *  @param[in] y_ intialisation value for x
     */
    coord_t(const geom_t x_, const geom_t y_) : x(x_), y(y_) {}
    /*! Default constructor (set both coordinates to 0)
     */
    coord_t() : x(geom_t(0)), y(geom_t(0)) {}
};

/*! A structure composed of two grid values to form a single grid point
 */
struct grid_t {
    llint il;  //!< The inline value
    llint xl;  //!< The crossline value

    /*! Constructor for initialising a grid point
     *  @param[in] i_ intialisation value for il
     *  @param[in] x_ intialisation value for xl
     */
    grid_t(const geom_t i_, const geom_t x_) : il(i_), xl(x_) {}

    /*! Default constructor (set both grid values to 0)
     */
    grid_t() : il(llint(0)), xl(llint(0)) {}
};

/*! @brief Possible coordinate sets
 */
enum class Coord : size_t {
    Src,  //!< Source Coordinates
    Rcv,  //!< Receiver Coordinates
    CMP   //!< Common Midpoint Coordinates
};

/*! @brief Possible Grids
 */
enum class Grid : size_t {
    Line  //!< Inline/Crossline grid points
};

/*! @brief Return a pair of coordinate offsets
 *  @param[in] pair The coordinate point type to return
 *  @return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
  std::pair<TrCrd, TrCrd>
  getPair(Coord pair)
{
    switch (pair) {
        case Coord::Src:
            return std::make_pair(TrCrd::xSrc, TrCrd::ySrc);
        case Coord::Rcv:
            return std::make_pair(TrCrd::xRcv, TrCrd::yRcv);
        default:
        case Coord::CMP:
            return std::make_pair(TrCrd::xCMP, TrCrd::yCMP);
    }
}

/*! @brief Return a pair of grid offsets
 *  @param[in] pair The grid point type to return
 *  @return The pair of offset enums
 */
#ifndef __ICC
constexpr
#else
inline
#endif
  std::pair<TrGrd, TrGrd>
  getPair(Grid pair)
{
    switch (pair) {
            //Note: When a new set of grid points are required:
            //        case Grid::OFR :
            //            return std::make_pair(TrHdr::ORF, TrHdr::TORF);
        default:
        case Grid::Line:
            return std::make_pair(TrGrd::il, TrGrd::xl);
    }
}

//extern geom_t scaleConv(int16_t scale);
geom_t getMd(const TrScal scal, const uchar* src);
geom_t getMd(const TrCrd item, const geom_t scale, const uchar* src);
int32_t getMd(const TrGrd item, const uchar* src);
void setCoord(
  const Coord item, const coord_t coord, const int16_t scale, uchar* buf);
coord_t getCoord(const Coord item, const geom_t scale, const uchar* buf);
grid_t getGrid(const Grid item, const uchar* buf);
void setGrid(const Grid item, const grid_t grid, uchar* buf);
//int16_t deScale(const geom_t val);
int16_t scalComp(int16_t scal1, int16_t scal2);
int16_t calcScale(const coord_t coord);
//void extractTraceParam(const uchar * md, TraceParam * prm);
//void insertTraceParam(const TraceParam * prm, uchar * md);
void setScale(
  const TrScal item, const int16_t scale, uchar* buf, size_t start = 0);

}  // namespace File
}  // namespace PIOL

#endif
