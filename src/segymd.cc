/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date October 2016
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
#include "file/segymd.hh"
#include "file/dynsegymd.hh"

namespace PIOL { namespace File {
/*! \brief Convert a SEG-Y scale integer to a floating point type
 *  \param[in] scale The int16_t scale taken from the SEG-Y file
 *  \return The scale convertered to floating point.
 */
geom_t scaleConv(int16_t scale)
{
    scale = (!scale ? 1 : scale);
    return (scale > 0 ? geom_t(scale) : geom_t(1)/geom_t(-scale));
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

/*! Extract the trace parameters from a character array and copy
 *  them to a TraceParam structure
 *  \param[in] md A charachter array of raw trace header contents
 *  \param[out] prm An array of TraceParam structures
 */
void extractTraceParam(const Rule * rule, size_t sz, const uchar * md, TraceParam * prm, size_t stride)
{
    if (!sz)
        return;

    DynParam dyn(rule, sz, stride);
    dyn.take(md);
    for (size_t i = 0; i < sz; i++)
    {
        prm[i].src.x = dyn.getPrm(i, Meta::xSrc);
        prm[i].src.y = dyn.getPrm(i, Meta::ySrc);
        prm[i].rcv.x = dyn.getPrm(i, Meta::xRcv);
        prm[i].rcv.y = dyn.getPrm(i, Meta::yRcv);
        prm[i].cmp.x = dyn.getPrm(i, Meta::xCmp);
        prm[i].cmp.y = dyn.getPrm(i, Meta::yCmp);
        prm[i].line.il = dyn.getPrm(i, Meta::il);
        prm[i].line.xl = dyn.getPrm(i, Meta::xl);

        prm[i].tn = dyn.getPrm(i, Meta::tn);
    }
}

/*! Insert the trace parameters from a TraceParam structure and
 *  copy them into a character array ready for writing to a segy file
 *  \param[in] prm An array of TraceParam structures
 *  \param[out] md A charachter array of raw trace header contents
 */
void insertTraceParam(const Rule * rule, size_t sz, const TraceParam * prm, uchar * md, size_t stride)
{
    if (!sz)
        return;
    DynParam dyn(rule, sz, stride);
    for (size_t i = 0; i < sz; i++)
    {
        dyn.setPrm(i, Meta::xSrc, prm[i].src.x);
        dyn.setPrm(i, Meta::ySrc, prm[i].src.y);
        dyn.setPrm(i, Meta::xRcv, prm[i].rcv.x);
        dyn.setPrm(i, Meta::yRcv, prm[i].rcv.y);
        dyn.setPrm(i, Meta::xCmp, prm[i].cmp.x);
        dyn.setPrm(i, Meta::yCmp, prm[i].cmp.y);
        dyn.setPrm(i, Meta::il, prm[i].line.il);
        dyn.setPrm(i, Meta::xl, prm[i].line.xl);
        dyn.setPrm(i, Meta::tn, llint(prm[i].tn));
    }
    dyn.fill(md);
}
}}
