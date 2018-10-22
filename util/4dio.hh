////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTIL_4DIO_HH
#define EXSEISDAT_UTIL_4DIO_HH

#include "4d.hh"

#include <limits>

namespace exseis {
namespace piol {
namespace four_d {

/// @brief Wrapper around posix_memalign testing its return value.
///
/// @tparam Args The input argument parameter list type.
/// @param[in] args A parameter pack of arguments, passed directly to
///                 posix_memalign.
///
template<typename... Args>
void checked_posix_memalign(Args... args)
{
    int err = posix_memalign(args...);

    switch (err) {
        case 0:
            return;

        case EINVAL:
            throw "posix_memalign size argument was not a power of 2, or not a multiple of sizeof(void*).";

        case ENOMEM:
            throw "Ran out of memory during posix_memalign allocation.";

        default:
            throw "Unknown error during posix_memalign";
    }
}

/*! This structure is for holding EXSEISDAT_ALIGN aligned memory containing the
 *  coordinates.
 */
struct Coords {
    /// The number of sets of coordinates
    size_t sz;

    /// The x src coordinates
    fourd_t* x_src = nullptr;

    /// The y src coordinates
    fourd_t* y_src = nullptr;

    /// The x rcv coordinates
    fourd_t* x_rcv = nullptr;

    /// The y rcv coordinates
    fourd_t* y_rcv = nullptr;

    /// The trace number
    size_t* tn = nullptr;

    /// The size which was actually allocated
    size_t alloc_size;

    /// The inline number
    exseis::utils::Integer* il = nullptr;

    /// The crossline number
    exseis::utils::Integer* xl = nullptr;

    /*! Constructor for coords. Allocate each array to take sz entries
     *  but also make sure that the allocated space is aligned.
     *  @param[in] sz     Number of traces
     *  @param[in] ixline Inline/Xline enabled
     */
    Coords(size_t sz, bool ixline) : sz(sz)
    {
        alloc_size =
          ((sz + EXSEISDAT_ALIGN) / EXSEISDAT_ALIGN) * EXSEISDAT_ALIGN;

        // posix_memalign() guarantees the memory allocated is alligned
        // according  to the alignment value
        checked_posix_memalign(
          reinterpret_cast<void**>(&x_src), EXSEISDAT_ALIGN,
          alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
          reinterpret_cast<void**>(&y_src), EXSEISDAT_ALIGN,
          alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
          reinterpret_cast<void**>(&x_rcv), EXSEISDAT_ALIGN,
          alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
          reinterpret_cast<void**>(&y_rcv), EXSEISDAT_ALIGN,
          alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
          reinterpret_cast<void**>(&tn), EXSEISDAT_ALIGN,
          alloc_size * sizeof(size_t));

        if (ixline) {
            checked_posix_memalign(
              reinterpret_cast<void**>(&il), EXSEISDAT_ALIGN,
              alloc_size * sizeof(exseis::utils::Integer));
            checked_posix_memalign(
              reinterpret_cast<void**>(&xl), EXSEISDAT_ALIGN,
              alloc_size * sizeof(exseis::utils::Integer));
        }
        for (size_t i = 0; i < alloc_size; i++) {
            x_src[i] = y_src[i] = x_rcv[i] = y_rcv[i] =
              std::numeric_limits<fourd_t>::max();
        }
        for (size_t i = 0; ixline && i < alloc_size; i++) {
            il[i] = xl[i] = std::numeric_limits<exseis::utils::Integer>::max();
        }
    }

    /*! Destructor. Deallocate all the memory.
     */
    ~Coords(void)
    {
        free(x_src);
        free(y_src);
        free(x_rcv);
        free(y_rcv);
        free(tn);
        free(il);
        free(xl);
    }
};

/*! Given an input file, perform a decomposition over the traces and return a
 *  subset of the coordinates to each process. The coordinates returned have
 *  been sorted into x_src order.
 *  @param[in] piol The piol object (in a shared pointer).
 *  @param[in] name The name of the input file.
 *  @param[in] ixline Inline/Xline enabled
 *  @return Return a unique_ptr to the structure containing the coordinates read
 *          by the local process.
 */
std::unique_ptr<Coords> get_coords(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, bool ixline);

/*! Extract traces and coordinates from an input file \c sname according to what
 *  traces are listed in \c list.
 *  @param[in] piol The piol object (in a shared pointer).
 *  @param[in] dname The name of the destination file.
 *  @param[in] sname The name of the source file.
 *  @param[in] list The list of traces to read from the input file in the order
 *                  they should appear in the output file.
 *  @param[in] minrs The value of minrs which should be stored with the trace
 *                   header of each trace.
 *  @param[in] print_dsr Print the dsdr value if true.
 */
void output_non_mono(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string dname,
  std::string sname,
  std::vector<size_t>& list,
  std::vector<fourd_t>& minrs,
  bool print_dsr);

}  // namespace four_d
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_UTIL_4DIO_HH
