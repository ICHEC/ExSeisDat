/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date October 2016
 *   @brief Contains functions which have become redundant in the main code but can be used
 *   as an alternative implementation for test purposes
 *   @details
 *//*******************************************************************************************/
/*#include <cstring>
#include <vector>
#include <memory>
#include <cmath>
#include <limits>
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "share/segy.hh"
#include "file/characterconversion.hh"
#include "share/units.hh"
#include "share/datatype.hh"
#include "file/segymd.hh"
*/
#include "cppfileapi.hh"
#include "segymdextra.hh"

namespace PIOL { namespace File {
/*! @brief Get the specified scale multipler from the Trace header.
 *  @param[in] scal The scalar of interest.
 *  @param[in] src The buffer of the header object.
 *  @return Return the scalar value.
 *
 *  @details If the integer value on disk is negative, the inverse
 *  of the absolute value is returned. If the value is zero,
 *  1 is returned, otherwise the value is returned. No check is done
 *  to ensure other restrictions are in place (i.e 1, 10, 1000 etc).
 */
geom_t getMd(const TrScal scal, const uchar * src)
{
    auto scale = getHost<int16_t>(&src[size_t(scal)-1U]);
    return scaleConv(scale);
}

/*! @brief Get the specified coordinate from the Trace header.
 *  @param[in] item The specific coordinate to get
 *  @param[in] scale The scale factor
 *  @param[in] src The buffer of the header object.
 *  @return Return the coordinate
 */
geom_t getMd(const TrCrd item, const geom_t scale, const uchar * src)
{
    return scale * geom_t(getHost<int32_t>(&src[size_t(item)-1U]));
}

/*! @brief Get the specified grid component from the Trace header.
 *  @param[in] item The specific grid component to get.
 *  @param[in] src The buffer of the header object.
 *  @return Return the grid component (two components make a grid point).
 */
int32_t getMd(const TrGrd item, const uchar * src)
{
    return getHost<int32_t>(&src[size_t(item)-1U]);
}


/*! @brief Set a trace scale in the trace header
 *  @param[in] item The scale item of interest
 *  @param[in] scale The metadata value to insert into the buffer.
 *  @param[in, out] buf The trace header as an array of uchar.
 */
void setScale(const TrScal item, const int16_t scale, uchar * buf, size_t start)
{
    getBigEndian(scale, &buf[size_t(item)-start-1U]);
}

/*! @brief Set a coordinate point in the trace header
 *  @param[in] item The coordinate point type of interest
 *  @param[in] coord The value of the coordinate point
 *  @param[in] scale The scale as an integer from the SEG-Y header
 *  @param[in, out] buf The trace header as an array of uchar.
 */
void setCoord(const Coord item, const coord_t coord, const int16_t scale, uchar * buf)
{
    auto pair = getPair(item);
    geom_t gscale = scaleConv(scale);
    getBigEndian(int32_t(std::lround(coord.x / gscale)), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(std::lround(coord.y / gscale)), &buf[size_t(pair.second) - 1U]);
}

//TODO: unit test
/*! @brief Get a coordinate point from the trace header
 *  @param[in] item The coordinate point type of interest
 *  @param[in] scale The scale from the SEG-Y header
 *  @param[in] buf A buffer containing the trace header
 *  @param[in, out] buf The trace header as an array of uchar.
 *  @return The coordinate point associated with the coordinate item
 */
coord_t getCoord(const Coord item, const geom_t scale, const uchar * buf)
{
    auto p = getPair(item);
    return coord_t(getMd(p.first, scale, buf),
                   getMd(p.second, scale, buf));
}

/*! @brief Get a grid point from the trace header
 *  @param[in] item The grid type of interest
 *  @param[in] buf A buffer containing the trace header
 *  @param[in, out] buf The trace header as an array of uchar.
 *  @return The grid point associated with the requested grid
 */
grid_t getGrid(const Grid item, const uchar * buf)
{
    auto p = getPair(item);
    return grid_t(getMd(p.first, buf),
                  getMd(p.second, buf));
}

/*! @brief Set a grid point in the trace header
 *  @param[in] item The grid point type of interest
 *  @param[in] grid The value of the grid point
 *  @param[out] buf The trace header as an array of uchar.
 */
void setGrid(const Grid item, const grid_t grid, uchar * buf)
{
    auto pair = getPair(item);
    getBigEndian(int32_t(grid.il), &buf[size_t(pair.first) - 1U]);
    getBigEndian(int32_t(grid.xl), &buf[size_t(pair.second) - 1U]);
}

/*! Compare two scales and return the appropriate one which maximises precision
 *  while preventing overflow of the int32_t type.
 *  @param[in] scal1 The first scale value
 *  @param[in] scal2 The second scale value
 *  @return The scal value which meets the precision criteria.
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
 *  @param[in] coord A coordinate point
 *  @return The extracted scal value which meets the precision criteria.
 */
int16_t calcScale(const coord_t coord)
{
    //I get the minimum value so that I definitely store the result.
    //This is at the expense of precision.
    int16_t scal1 = deScale(coord.x);
    int16_t scal2 = deScale(coord.y);
    return scalComp(scal1, scal2);
}

/*! Extract the trace parameters from a character array and copy
 *  them to a TraceParam structure
 *  @param[in] md A charachter array of raw trace header contents
 *  @param[out] prm An array of TraceParam structures
 */
/*void extractTraceParam(const uchar * md, TraceParam * prm)
{
    geom_t scale = getMd(TrScal::ScaleCoord, md);
    prm->src = getCoord(Coord::Src, scale, md);
    prm->rcv = getCoord(Coord::Rcv, scale, md);
    prm->cmp = getCoord(Coord::CMP, scale, md);
    prm->line = getGrid(Grid::Line, md);
    prm->tn = getHost<int32_t>(&md[size_t(TrHdr::SeqFNum)-1]);
}*/

/*! Insert the trace parameters from a TraceParam structure and
 *  copy them into a character array ready for writing to a segy file
 *  @param[in] prm An array of TraceParam structures
 *  @param[out] md A charachter array of raw trace header contents
 */
/*void insertTraceParam(const TraceParam * prm, uchar * md)
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
}*/
}}
