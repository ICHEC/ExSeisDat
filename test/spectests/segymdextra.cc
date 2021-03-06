////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Contains functions which have become redundant in the main code but
///        can be used as an alternative implementation for test purposes
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "segymdextra.hh"

#include "ExSeisDat/PIOL/segy_utils.hh"

using namespace exseis::utils;

namespace exseis {
namespace PIOL {

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
exseis::utils::Floating_point getMd(const TrScal scal, const unsigned char* src)
{
    auto scale = from_big_endian<int16_t>(
      src[size_t(scal) - 1U + 0], src[size_t(scal) - 1U + 1]);

    return SEGY_utils::parse_scalar(scale);
}

/*! @brief Get the specified coordinate from the Trace header.
 *  @param[in] item The specific coordinate to get
 *  @param[in] scale The scale factor
 *  @param[in] src The buffer of the header object.
 *  @return Return the coordinate
 */
exseis::utils::Floating_point getMd(
  const TrCrd item,
  const exseis::utils::Floating_point scale,
  const unsigned char* src)
{
    return scale
           * exseis::utils::Floating_point(from_big_endian<int32_t>(
               src[size_t(item) - 1U + 0], src[size_t(item) - 1U + 1],
               src[size_t(item) - 1U + 2], src[size_t(item) - 1U + 3]));
}

/*! @brief Get the specified grid component from the Trace header.
 *  @param[in] item The specific grid component to get.
 *  @param[in] src The buffer of the header object.
 *  @return Return the grid component (two components make a grid point).
 */
int32_t getMd(const TrGrd item, const unsigned char* src)
{
    return from_big_endian<int32_t>(
      src[size_t(item) - 1U + 0], src[size_t(item) - 1U + 1],
      src[size_t(item) - 1U + 2], src[size_t(item) - 1U + 3]);
}

/*! @brief Set a trace scale in the trace header
 *  @param[in] item The scale item of interest
 *  @param[in] scale The metadata value to insert into the buffer.
 *  @param[in, out] buf The trace header as an array of unsigned char.
 */
void setScale(
  const TrScal item, const int16_t scale, unsigned char* buf, size_t start)
{
    const auto be_scale = to_big_endian(scale);

    std::copy(
      std::begin(be_scale), std::end(be_scale),
      &buf[size_t(item) - start - 1U]);
}

/*! @brief Set a coordinate point in the trace header
 *  @param[in] item The coordinate point type of interest
 *  @param[in] coord The value of the coordinate point
 *  @param[in] scale The scale as an integer from the SEG-Y header
 *  @param[in, out] buf The trace header as an array of unsigned char.
 */
void setCoord(
  const Coord item,
  const coord_t coord,
  const int16_t scale,
  unsigned char* buf)
{
    auto pair                            = getPair(item);
    exseis::utils::Floating_point gscale = SEGY_utils::parse_scalar(scale);

    const auto be_scaled_x =
      to_big_endian(int32_t(std::lround(coord.x / gscale)));

    const auto be_scaled_y =
      to_big_endian(int32_t(std::lround(coord.y / gscale)));

    std::copy(
      std::begin(be_scaled_x), std::end(be_scaled_x),
      &buf[size_t(pair.first) - 1U]);

    std::copy(
      std::begin(be_scaled_y), std::end(be_scaled_y),
      &buf[size_t(pair.second) - 1U]);
}

// TODO: unit test
/*! @brief Get a coordinate point from the trace header
 *  @param[in] item The coordinate point type of interest
 *  @param[in] scale The scale from the SEG-Y header
 *  @param[in] buf A buffer containing the trace header
 *  @param[in, out] buf The trace header as an array of unsigned char.
 *  @return The coordinate point associated with the coordinate item
 */
coord_t getCoord(
  const Coord item,
  const exseis::utils::Floating_point scale,
  const unsigned char* buf)
{
    auto p = getPair(item);
    return coord_t(getMd(p.first, scale, buf), getMd(p.second, scale, buf));
}

/*! @brief Get a grid point from the trace header
 *  @param[in] item The grid type of interest
 *  @param[in] buf A buffer containing the trace header
 *  @param[in, out] buf The trace header as an array of unsigned char.
 *  @return The grid point associated with the requested grid
 */
grid_t getGrid(const Grid item, const unsigned char* buf)
{
    auto p = getPair(item);
    return grid_t(getMd(p.first, buf), getMd(p.second, buf));
}

/*! @brief Set a grid point in the trace header
 *  @param[in] item The grid point type of interest
 *  @param[in] grid The value of the grid point
 *  @param[out] buf The trace header as an array of unsigned char.
 */
void setGrid(const Grid item, const grid_t grid, unsigned char* buf)
{
    auto pair = getPair(item);

    const auto be_il = to_big_endian(int32_t(grid.il));
    const auto be_xl = to_big_endian(int32_t(grid.xl));

    std::copy(
      std::begin(be_il), std::end(be_il), &buf[size_t(pair.first) - 1U]);
    std::copy(
      std::begin(be_xl), std::end(be_xl), &buf[size_t(pair.second) - 1U]);
}

/*! Compare two scales and return the appropriate one which maximises precision
 *  while preventing overflow of the int32_t type.
 *  @param[in] scal1 The first scale value
 *  @param[in] scal2 The second scale value
 *  @return The scal value which meets the precision criteria.
 */
int16_t scalComp(int16_t scal1, int16_t scal2)
{
    // if the scale is bigger than 1 that means we need to use the largest
    // to ensure conservation of the most significant digit
    // otherwise we choose the scale that preserves the most digits
    // after the decimal place.
    if (scal1 > 1 || scal2 > 1) {
        return std::max(scal1, scal2);
    }
    else {
        return std::min(scal1, scal2);
    }
}

/*! Extract the scale value from each coordinate from a coordinate point
 *  and return the most appropriate scale value that maximises precision
 *  while preventing overflow of the int32_t type.
 *  @param[in] coord A coordinate point
 *  @return The extracted scal value which meets the precision criteria.
 */
int16_t calcScale(const coord_t coord)
{
    // I get the minimum value so that I definitely store the result.
    // This is at the expense of precision.
    int16_t scal1 = SEGY_utils::find_scalar(coord.x);
    int16_t scal2 = SEGY_utils::find_scalar(coord.y);
    return scalComp(scal1, scal2);
}

// /*! Extract the trace parameters from a character array and copy
//  *  them to a TraceParam structure
//  *  @param[in] md A charachter array of raw trace header contents
//  *  @param[out] prm An array of TraceParam structures
//  */
// void extractTraceParam(const unsigned char * md, TraceParam * prm)
// {
//     exseis::utils::Floating_point scale = getMd(TrScal::ScaleCoord, md);
//     prm->src = getCoord(Coord::Src, scale, md);
//     prm->rcv = getCoord(Coord::Rcv, scale, md);
//     prm->cmp = getCoord(Coord::CMP, scale, md);
//     prm->line = getGrid(Grid::Line, md);
//     prm->tn = from_big_endian<int32_t>(&md[size_t(TrHdr::SeqFNum)-1]);
// }

// /*! Insert the trace parameters from a TraceParam structure and
//  *  copy them into a character array ready for writing to a segy file
//  *  @param[in] prm An array of TraceParam structures
//  *  @param[out] md A charachter array of raw trace header contents
//  */
// void insertTraceParam(const TraceParam * prm, unsigned char * md)
// {
//     int16_t scale = scalComp(1, calcScale(prm->src));
//     scale = scalComp(scale, calcScale(prm->rcv));
//     scale = scalComp(scale, calcScale(prm->cmp));
//     setScale(TrScal::ScaleCoord, scale, md);
//
//     setCoord(Coord::Src, prm->src, scale, md);
//     setCoord(Coord::Rcv, prm->rcv, scale, md);
//     setCoord(Coord::CMP, prm->cmp, scale, md);
//     setGrid(Grid::Line, prm->line, md);
//
//     //narrowing conversion of tn
//     const auto be_tn = to_big_endian(int32_t(prm->tn));
//     std::copy(
//       std::begin(be_tn), std::end(be_tn), &md[size_t(TrHdr::SeqFNum) - 1U]);
// }

}  // namespace PIOL
}  // namespace exseis
