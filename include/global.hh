/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date July 2016
 *   @brief
 *   @details Insert trivial typdefs etc into this file which will be used throughout the
 *            code-base.
 *//*******************************************************************************************/
#ifndef PIOLGLOBAL_INCLUDE_GUARD
#define PIOLGLOBAL_INCLUDE_GUARD

#include "stddef.h"

#ifdef __cplusplus

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <complex>

extern "C" {
#endif // __cplusplus


typedef ptrdiff_t PIOL_llint;
typedef float     PIOL_trace_t;
typedef double    PIOL_geom_t;

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#ifdef __cplusplus

namespace PIOL {

    typedef unsigned char uchar;        //!< The type to use for accessing individual bytes
    typedef PIOL_llint   llint;         //!< A large signed integer type.
    typedef PIOL_trace_t trace_t;       //!< Real type for traces, use "*_t" convention
    typedef PIOL_geom_t  geom_t;        //!< Real type for geometry
    typedef double unit_t;              //!< Real type for physical, SI and math units

    typedef std::complex<trace_t> cmtrace_t;              //!< Real type for traces, use "*_t" convention
    constexpr trace_t operator "" _t(long double x)
    {
        return x;
    }
    constexpr trace_t operator "" _t(unsigned long long int i)
    {
        return i;
    }

}

namespace PIOL {

    /*! The file modes possible for files.
     */
    enum class FileMode : size_t
    {
        Read,       //!< Read-only mode
        Write,      //!< Write-only mode
        ReadWrite,  //!< Read or write
        Test        //!< A test mode
    };

    typedef std::function<trace_t(trace_t *, size_t, trace_t, llint)> AGCFunc;          //!< A typedef for the agc stats function
    typedef std::function<trace_t(trace_t, trace_t)> TaperFunc;                         //!< A typedef for the agc stats function
    typedef std::function<trace_t(trace_t *, size_t, size_t, size_t)> FltrPad;  //!< Typedef for filter padding funcitons
    template <typename T>
    using MinMaxFunc = std::function<geom_t(const T &)>;                                //!< Return the value associated with a particular parameter
    template <class T>
    using Compare = std::function<bool(const T &, const T &)>;                          //!< A template for the Compare less-than function

    namespace Data { class Interface; }
    namespace Obj { class Interface; }
    namespace File { class ReadInterface; class WriteInterface; }
}

#endif // __cplusplus

#endif // PIOLGLOBAL_INCLUDE_GUARD
