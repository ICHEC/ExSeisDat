////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FOUR_D_4DIO_HH
#define EXSEIS_PIOL_FOUR_D_4DIO_HH

#include "4d.hh"

#include "exseis/utils/communicator/Communicator_mpi.hh"

#include <limits>

namespace exseis {
namespace apps {
inline namespace fourdbin {

/// @brief Wrapper around posix_memalign testing its return value.
///
/// @tparam Args The input argument parameter list type.
///
/// @param[in]  args    A parameter pack of arguments, passed directly to
///                     posix_memalign.
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

/// @brief This structure is for holding EXSEIS_ALIGN aligned memory
///        containing the coordinates.
///
struct Coords {
    /// @brief The number of sets of coordinates
    size_t sz;

    /// @brief The x src coordinates
    fourd_t* x_src = nullptr;

    /// @brief The y src coordinates
    fourd_t* y_src = nullptr;

    /// @brief The x rcv coordinates
    fourd_t* x_rcv = nullptr;

    /// @brief The y rcv coordinates
    fourd_t* y_rcv = nullptr;

    /// @brief The trace number
    size_t* tn = nullptr;

    /// @brief The size which was actually allocated
    size_t alloc_size;

    /// @brief The inline number
    Integer* il = nullptr;

    /// @brief The crossline number
    Integer* xl = nullptr;

    /// @brief Constructor for coords.
    ///
    /// @param[in] sz     Number of traces
    /// @param[in] ixline Inline/Xline enabled
    ///
    /// @details Allocate each array to take sz entries but also make sure that
    /// the allocated space is aligned.
    ///
    Coords(size_t sz, bool ixline) : sz(sz)
    {
        alloc_size = ((sz + EXSEIS_ALIGN) / EXSEIS_ALIGN) * EXSEIS_ALIGN;

        // posix_memalign() guarantees the memory allocated is alligned
        // according  to the alignment value
        checked_posix_memalign(
            reinterpret_cast<void**>(&x_src), EXSEIS_ALIGN,
            alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
            reinterpret_cast<void**>(&y_src), EXSEIS_ALIGN,
            alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
            reinterpret_cast<void**>(&x_rcv), EXSEIS_ALIGN,
            alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
            reinterpret_cast<void**>(&y_rcv), EXSEIS_ALIGN,
            alloc_size * sizeof(fourd_t));
        checked_posix_memalign(
            reinterpret_cast<void**>(&tn), EXSEIS_ALIGN,
            alloc_size * sizeof(size_t));

        if (ixline) {
            checked_posix_memalign(
                reinterpret_cast<void**>(&il), EXSEIS_ALIGN,
                alloc_size * sizeof(Integer));
            checked_posix_memalign(
                reinterpret_cast<void**>(&xl), EXSEIS_ALIGN,
                alloc_size * sizeof(Integer));
        }
        for (size_t i = 0; i < alloc_size; i++) {
            x_src[i] = y_src[i] = x_rcv[i] = y_rcv[i] =
                std::numeric_limits<fourd_t>::max();
        }
        for (size_t i = 0; ixline && i < alloc_size; i++) {
            il[i] = xl[i] = std::numeric_limits<Integer>::max();
        }
    }

    /// @brief Destructor.
    ///
    /// @details Deallocate all the memory.
    ~Coords()
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

/// @brief Perform decomposition and return coordinates
///
/// @param[in] communicator  The communicator to decompose over
/// @param[in] name     The name of the input file.
/// @param[in] ixline   Inline/Xline enabled
///
/// @return Return a unique_ptr to the structure containing the coordinates read
///         by the local process.
///
/// @details Given an input file, perform a decomposition over the traces and
///          return a subset of the coordinates to each process. The coordinates
///          returned have been sorted into x_src order.
///
std::unique_ptr<Coords> get_coords(
    const Communicator_mpi& communicator, std::string name, bool ixline);


/// @brief Extract traces and coordinates from an input file \c sname according
///        to what traces are listed in \c list.
///
/// @param[in] communicator  The communicator to output over
/// @param[in] dname The name of the destination file.
/// @param[in] sname The name of the source file.
/// @param[in] list The list of traces to read from the input file in the order
///            they should appear in the output file.
/// @param[in] minrs The value of minrs which should be stored with the trace
///            header of each trace.
/// @param[in] print_dsr Print the dsdr value if true.
void output_non_mono(
    const Communicator_mpi& communicator,
    std::string dname,
    std::string sname,
    std::vector<size_t>& list,
    std::vector<fourd_t>& minrs,
    bool print_dsr);

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis

#endif  // EXSEIS_PIOL_FOUR_D_4DIO_HH
