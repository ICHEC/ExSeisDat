////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date October 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLFILETESTSEGYMD_INCLUDE_GUARD
#define PIOLFILETESTSEGYMD_INCLUDE_GUARD

#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/utils/encoding/character_encoding.hh"
#include "ExSeisDat/utils/encoding/number_encoding.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

/*! Misc Trace Header offsets
 */
enum class TrHdr : size_t {
    /// int32_t. The trace sequence number within the Line
    SeqNum = 1U,
    /// int32_t. The trace sequence number within SEG-Y File
    SeqFNum = 5U,
    /// int32_t. The original field record number.
    ORF = 9U,
    /// int32_t. The trace number within the ORF.
    TORF = 13U
};

/*! Trace Header offsets to elevations
 */
enum class TrElev : size_t {
    /// int32_t. The Receiver group elevation
    RcvElv = 41U,
    /// int32_t. The surface elevation at the source.
    SurfElvSrc = 45U,
    /// int32_t. The source depth below surface (opposite of above?).
    SrcDpthSurf = 49U,
    /// int32_t. The datum elevation for the receiver group.
    DtmElvRcv = 53U,
    /// int32_t. The datum elevation for the source.
    DtmElvSrc = 57U,
    /// int32_t. The water depth for the source.
    WtrDepSrc = 61U,
    /// int32_t. The water depth for the receive group.
    WtrDepRcv = 65U
};

/*! Trace Header offsets to scalers
 */
enum class TrScal : size_t {
    /// int16_t. The scale coordinate for 41-68 (elevations + depths)
    ScaleElev = 69U,
    /// int16_t. The scale coordinate for 73-88 + 181-188
    ScaleCoord = 71U
};

/*! Trace Header offsets to coordinates
 */
enum class TrCrd : size_t {
    /// int32_t. The X coordinate for the source
    xSrc = 73U,
    /// int32_t. The Y coordinate for the source
    ySrc = 77U,
    /// int32_t. The X coordinate for the receive group
    xRcv = 81U,
    /// int32_t. The Y coordinate for the receive group
    yRcv = 85U,
    /// int32_t  The X coordinate for the CMP
    xCMP = 181U,
    /// int32_t. The Y coordinate for the CMP
    yCMP = 185U
};

/*! Trace Header offsets to grid components
 */
enum class TrGrd : size_t {
    /// int32_t. The Inline grid point.
    il = 189U,
    /// int32_t. The Crossline grid point.
    xl = 193U
};

struct coord_t {
    /// The x coordinate
    exseis::utils::Floating_point x;
    /// The y coordinate
    exseis::utils::Floating_point y;

    /*! Constructor for initialising coordinates
     *  @param[in] x_ intialisation value for x
     *  @param[in] y_ intialisation value for x
     */
    coord_t(
      exseis::utils::Floating_point x_, exseis::utils::Floating_point y_) :
        x(x_),
        y(y_)
    {
    }

    /*! Default constructor (set both coordinates to 0)
     */
    coord_t() :
        x(exseis::utils::Floating_point(0)),
        y(exseis::utils::Floating_point(0))
    {
    }
};

/*! A structure composed of two grid values to form a single grid point
 */
struct grid_t {
    /// The inline value
    exseis::utils::Integer il;
    /// The crossline value
    exseis::utils::Integer xl;

    /*! Constructor for initialising a grid point
     *  @param[in] i_ intialisation value for il
     *  @param[in] x_ intialisation value for xl
     */
    grid_t(exseis::utils::Floating_point i_, exseis::utils::Floating_point x_) :
        il(i_),
        xl(x_)
    {
    }

    /*! Default constructor (set both grid values to 0)
     */
    grid_t() : il(exseis::utils::Integer(0)), xl(exseis::utils::Integer(0)) {}
};

/*! @brief Possible coordinate sets
 */
enum class Coord : size_t {
    /// Source Coordinates
    Src,
    /// Receiver Coordinates
    Rcv,
    /// Common Midpoint Coordinates
    CMP
};

/*! @brief Possible Grids
 */
enum class Grid : size_t {
    /// Inline/Crossline grid points
    Line
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
        // Note: When a new set of grid points are required:
        //        case Grid::OFR :
        //            return std::make_pair(TrHdr::ORF, TrHdr::TORF);
        default:
        case Grid::Line:
            return std::make_pair(TrGrd::il, TrGrd::xl);
    }
}

exseis::utils::Floating_point getMd(TrScal scal, unsigned char* src);

exseis::utils::Floating_point getMd(
  TrCrd item, exseis::utils::Floating_point scale, const unsigned char* src);

int32_t getMd(TrGrd item, const unsigned char* src);

void setCoord(Coord item, coord_t coord, int16_t scale, unsigned char* buf);

coord_t getCoord(
  Coord item, exseis::utils::Floating_point scale, const unsigned char* buf);

grid_t getGrid(Grid item, const unsigned char* buf);

void setGrid(Grid item, grid_t grid, unsigned char* buf);

int16_t scalComp(int16_t scal1, int16_t scal2);

int16_t calcScale(coord_t coord);

void setScale(TrScal item, int16_t scale, unsigned char* buf, size_t start = 0);

}  // namespace PIOL
}  // namespace exseis

#endif
