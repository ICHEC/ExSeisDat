////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Sort Operation
/// @details The algorithm used is a nearest neighbour approach where at each
///          iteration the lowest valued metadata entries are moved to adjacent
///          processes with a lower rank and a sort is performed. After the sort
///          the highest entries are moved again to the process with a higher
///          rank. If each process has the same traces it started off with, the
///          sort is complete.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/operations/sort.hh"

#include "exseis/utils/mpi_utils/MPI_error_to_string.hh"
#include "exseis/utils/types/MPI_type.hh"
#include "exseis/utils/types/Type.hh"
#include "exseis/utils/types/typedefs.hh"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>
#include <vector>

#include <mpi.h>

using namespace std::string_literals;

namespace exseis {
inline namespace piol {
inline namespace operations {

std::vector<size_t> get_sort_index(size_t sz, const size_t* list)
{
    std::vector<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list](size_t s1, size_t s2) {
        return list[s1] < list[s2];
    });
    return index;
}

/*! Calculate the square of the hypotenuse
 *  @param[in] sx The source x coordinate
 *  @param[in] sy The source y coordinate
 *  @param[in] rx The receiver x coordinate
 *  @param[in] ry The receiver y coordinate
 *  @return square of the hypotenuse
 */
inline Floating_point off(
    Floating_point sx, Floating_point sy, Floating_point rx, Floating_point ry)
{
    return (sx - rx) * (sx - rx) + (sy - ry) * (sy - ry);
}

/*! For sorting by Src X, Src Y, Rcv X, Rcv Y.
 *  @param[in] trace_metadata The parameter structure
 *  @param[in] i   Structure to access 0th parameter of associated Trace_metadata struct.
 *  @param[in] j   Structure to access 0th parameter of associated Trace_metadata struct.
 *  @return Return true if entry \c i of \p trace_metadata is less than entry \c j in terms
 *          of the sort.
 */
static bool less_src_rcv(
    const Trace_metadata& trace_metadata, const size_t i, const size_t j)
{
    auto e1sx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
    auto e2sx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);

    if (e1sx < e2sx) {
        return true;
    }
    if (e1sx > e2sx) {
        return false;
    }

    // e1sz == e2sx

    auto e1sy = trace_metadata.get_floating_point(i, Trace_metadata_key::y_src);
    auto e2sy = trace_metadata.get_floating_point(j, Trace_metadata_key::y_src);

    if (e1sy < e2sy) {
        return true;
    }
    if (e1sy > e2sy) {
        return false;
    }

    // e1sy == e2sy

    auto e1rx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_rcv);
    auto e2rx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_rcv);

    if (e1rx < e2rx) {
        return true;
    }
    if (e1rx > e2rx) {
        return false;
    }

    // e1rx == e2rx

    auto e1ry = trace_metadata.get_floating_point(i, Trace_metadata_key::y_rcv);
    auto e2ry = trace_metadata.get_floating_point(j, Trace_metadata_key::y_rcv);

    if (e1ry < e2ry) {
        return true;
    }
    if (e1ry > e2ry) {
        return false;
    }

    // e1ry == e2ry

    return trace_metadata.get_index(i, Trace_metadata_key::ltn)
           < trace_metadata.get_index(j, Trace_metadata_key::ltn);
}

/*! For sorting by Src X, Src Y and Offset.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] trace_metadata     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @return Return true if entry \c i of \p trace_metadata is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool less_src_off(
    const Trace_metadata& trace_metadata, const size_t i, const size_t j)
{
    auto e1sx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
    auto e2sx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);

    if (e1sx < e2sx) {
        return true;
    }
    if (e1sx > e2sx) {
        return false;
    }

    // e1sz == e2sx

    auto e1sy = trace_metadata.get_floating_point(i, Trace_metadata_key::y_src);
    auto e2sy = trace_metadata.get_floating_point(j, Trace_metadata_key::y_src);

    if (e1sy < e2sy) {
        return true;
    }
    if (e1sy > e2sy) {
        return false;
    }

    // e1sy == e2sy

    auto e1rx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_rcv);
    auto e1ry = trace_metadata.get_floating_point(i, Trace_metadata_key::y_rcv);
    auto off1 =
        (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                   trace_metadata.get_integer(i, Trace_metadata_key::Offset));

    auto e2rx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_rcv);
    auto e2ry = trace_metadata.get_floating_point(j, Trace_metadata_key::y_rcv);
    auto off2 =
        (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                   trace_metadata.get_integer(j, Trace_metadata_key::Offset));

    if (off1 < off2) {
        return true;
    }
    if (off1 > off2) {
        return false;
    }

    // off1 == off2

    auto ltn1 = trace_metadata.get_index(i, Trace_metadata_key::ltn);
    auto ltn2 = trace_metadata.get_index(j, Trace_metadata_key::ltn);

    return ltn1 < ltn2;
}

/*! For sorting by Rcv X, Rcv Y and Offset.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] trace_metadata     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @return Return true if entry \c i of \p trace_metadata is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool less_rcv_off(
    const Trace_metadata& trace_metadata, const size_t i, const size_t j)
{
    auto e1rx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_rcv);
    auto e2rx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_rcv);

    if (e1rx < e2rx) {
        return true;
    }
    if (e1rx > e2rx) {
        return false;
    }

    // e1rx == e2rx

    auto e1ry = trace_metadata.get_floating_point(i, Trace_metadata_key::y_rcv);
    auto e2ry = trace_metadata.get_floating_point(j, Trace_metadata_key::y_rcv);

    if (e1ry < e2ry) {
        return true;
    }
    if (e1ry > e2ry) {
        return false;
    }

    // e1ry == e2ry

    auto e1sx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
    auto e1sy = trace_metadata.get_floating_point(i, Trace_metadata_key::y_src);
    auto off1 =
        (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                   trace_metadata.get_integer(i, Trace_metadata_key::Offset));

    auto e2sx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);
    auto e2sy = trace_metadata.get_floating_point(j, Trace_metadata_key::y_src);
    auto off2 =
        (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                   trace_metadata.get_integer(j, Trace_metadata_key::Offset));

    if (off1 < off2) {
        return true;
    }
    if (off1 > off2) {
        return false;
    }

    // off1 == off2

    return trace_metadata.get_index(i, Trace_metadata_key::ltn)
           < trace_metadata.get_index(j, Trace_metadata_key::ltn);
}

/*! For sorting by Inline, Crossline and Offset.
 *  @tparam CalcOff If true, calculate the offset, otherwise read the offset
 *                  from the header
 *  @param[in] trace_metadata The parameter structure
 *  @param[in] i   Structure to access 0th parameter of associated Trace_metadata struct.
 *  @param[in] j   Structure to access 0th parameter of associated Trace_metadata struct.
 *  @return Return true if entry \c i of \p trace_metadata is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool less_line_off(
    const Trace_metadata& trace_metadata, const size_t i, const size_t j)
{
    auto e1il = trace_metadata.get_integer(i, Trace_metadata_key::il);
    auto e2il = trace_metadata.get_integer(j, Trace_metadata_key::il);

    if (e1il < e2il) {
        return true;
    }
    if (e1il > e2il) {
        return false;
    }

    // e1il == e2il

    auto e1xl = trace_metadata.get_integer(i, Trace_metadata_key::xl);
    auto e2xl = trace_metadata.get_integer(j, Trace_metadata_key::xl);

    if (e1xl < e2xl) {
        return true;
    }
    if (e1xl > e2xl) {
        return false;
    }

    // e1xl == e2xl

    auto e1sx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
    auto e1sy = trace_metadata.get_floating_point(i, Trace_metadata_key::y_src);
    auto e1rx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_rcv);
    auto e1ry = trace_metadata.get_floating_point(i, Trace_metadata_key::y_rcv);

    auto e2sx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);
    auto e2sy = trace_metadata.get_floating_point(j, Trace_metadata_key::y_src);
    auto e2rx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_rcv);
    auto e2ry = trace_metadata.get_floating_point(j, Trace_metadata_key::y_rcv);

    auto off1 =
        (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                   trace_metadata.get_integer(i, Trace_metadata_key::Offset));
    auto off2 =
        (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                   trace_metadata.get_integer(j, Trace_metadata_key::Offset));

    if (off1 < off2) {
        return true;
    }
    if (off1 > off2) {
        return false;
    }

    // off1 == off2

    return trace_metadata.get_index(i, Trace_metadata_key::ltn)
           < trace_metadata.get_index(j, Trace_metadata_key::ltn);
}

/*! For sorting by Offset, Inline and Crossline.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] trace_metadata     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Trace_metadata
 *                     struct.
 *  @return Return true if entry \c i of \p trace_metadata is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool less_off_line(
    const Trace_metadata& trace_metadata, const size_t i, const size_t j)
{
    auto e1sx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
    auto e1sy = trace_metadata.get_floating_point(i, Trace_metadata_key::y_src);
    auto e1rx = trace_metadata.get_floating_point(i, Trace_metadata_key::x_rcv);
    auto e1ry = trace_metadata.get_floating_point(i, Trace_metadata_key::y_rcv);
    auto off1 =
        (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                   trace_metadata.get_integer(i, Trace_metadata_key::Offset));

    auto e2sx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);
    auto e2sy = trace_metadata.get_floating_point(j, Trace_metadata_key::y_src);
    auto e2rx = trace_metadata.get_floating_point(j, Trace_metadata_key::x_rcv);
    auto e2ry = trace_metadata.get_floating_point(j, Trace_metadata_key::y_rcv);
    auto off2 =
        (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                   trace_metadata.get_integer(j, Trace_metadata_key::Offset));

    if (off1 < off2) {
        return true;
    }
    if (off1 > off2) {
        return false;
    }

    // off1 == off2

    auto e1il = trace_metadata.get_integer(i, Trace_metadata_key::il);
    auto e2il = trace_metadata.get_integer(j, Trace_metadata_key::il);

    if (e1il < e2il) {
        return true;
    }
    if (e1il > e2il) {
        return false;
    }

    // e1il == e2il

    auto e1xl = trace_metadata.get_integer(i, Trace_metadata_key::xl);
    auto e2xl = trace_metadata.get_integer(j, Trace_metadata_key::xl);

    if (e1xl < e2xl) {
        return true;
    }
    if (e1xl > e2xl) {
        return false;
    }

    // e1xl == e2xl
    return trace_metadata.get_index(i, Trace_metadata_key::ltn)
           < trace_metadata.get_index(j, Trace_metadata_key::ltn);
}

CompareP get_comp(Sort_type type)
{
    switch (type) {
        case Sort_type::SrcRcv:
            return less_src_rcv;

        case Sort_type::SrcOff:
            return less_src_off<true>;

        case Sort_type::SrcROff:
            return less_src_off<false>;

        case Sort_type::RcvOff:
            return less_rcv_off<true>;

        case Sort_type::RcvROff:
            return less_rcv_off<false>;

        case Sort_type::LineOff:
            return less_line_off<true>;

        case Sort_type::LineROff:
            return less_line_off<false>;

        case Sort_type::OffLine:
            return less_off_line<true>;

        case Sort_type::ROffLine:
            return less_off_line<false>;
    }

    assert(false && "getComp: Unknown Sort_type");
}

std::vector<size_t> sort(
    const Communicator_mpi& communicator,
    Sort_type type,
    Trace_metadata& trace_metadata)
{
    return sort(communicator, trace_metadata, get_comp(type));
}

bool check_order(
    const Input_file& src, Contiguous_decomposition dec, Sort_type type)
{
    auto comp = get_comp(type);
    Trace_metadata trace_metadata(dec.local_size);

    src.read_metadata(dec.global_offset, dec.local_size, trace_metadata);

    for (size_t i = 1; i < dec.local_size; i++) {
        if (!comp(trace_metadata, i - 1, i)) {
            return false;
        }
    }

    return true;
}

/**************************** Core Implementation *****************************/

namespace {

/*! wait on two requests to finish. The largest and smallest rank only wait on
 *  one request.
 *  @param[in] communicator  The Communication object to wait over
 *  @param[in] req1 The request that all processes except rank of num_rank-1 wait
 *                  on.
 *  @param[in] req2 The request that all processes except rank of 0 wait on..
 */
void wait(
    const Communicator_mpi& communicator,
    std::vector<MPI_Request> req1,
    std::vector<MPI_Request> req2)
{
    MPI_Status stat;
    int err;

    if (communicator.get_rank() != communicator.get_num_rank() - 1) {
        for (size_t i = 0; i < req1.size(); i++) {
            err = MPI_Wait(&req1[i], &stat);

            if (err != MPI_SUCCESS) {
                communicator.log()->add_entry(Log_entry{
                    Status::Error,
                    "Sort Rcv error: "s + mpi_error_to_string(err, &stat),
                    Verbosity::none,
                    EXSEIS_SOURCE_POSITION("exseis::piol::wait")});
            }
        }
    }

    if (communicator.get_rank() != 0) {
        for (size_t i = 0; i < req2.size(); i++) {
            err = MPI_Wait(&req2[i], &stat);

            if (err != MPI_SUCCESS) {
                communicator.log()->add_entry(Log_entry{
                    Status::Error,
                    "Sort Snd error: "s + mpi_error_to_string(err, &stat),
                    Verbosity::none,
                    EXSEIS_SOURCE_POSITION("exseis::piol::wait")});
            }
        }
    }
}

}  // namespace

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  @tparam T Type of vector
 *  @param[in] communicator  The Communicator object to send over
 *  @param[in] region_sz The size of data to send/receive.
 *  @param[in,out] dat Vector to be accessed
 */
template<class T>
void send_right(
    const Communicator_mpi& communicator, size_t region_sz, std::vector<T>& dat)
{
    // Check conversions are within range
    assert(communicator.get_rank() <= std::numeric_limits<int>::max());
    assert(region_sz * sizeof(T) <= std::numeric_limits<int>::max());

    int rank = static_cast<int>(communicator.get_rank());
    int cnt  = static_cast<int>(region_sz * sizeof(T));

    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    // TODO: Move to the communication layer?
    if (rank != 0) {
        int err = MPI_Irecv(
            dat.data(), cnt, MPI_CHAR, rank - 1, 1,
            communicator.mpi_communicator(), &rrcv[0]);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(
                Log_entry{Status::Error,
                          "Sort MPI_Irecv error: "s + mpi_error_to_string(err),
                          Verbosity::none,
                          EXSEIS_SOURCE_POSITION("exseis::piol::send_right")});
        }
    }

    assert(communicator.get_num_rank() <= std::numeric_limits<int>::max());

    if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {
        int err = MPI_Isend(
            &dat[dat.size() - region_sz], cnt, MPI_CHAR, rank + 1, 1,
            communicator.mpi_communicator(), &rsnd[0]);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(
                Log_entry{Status::Error,
                          "Sort MPI_Isend error: "s + mpi_error_to_string(err),
                          Verbosity::none,
                          EXSEIS_SOURCE_POSITION("exseis::piol::send_right")});
        }
    }

    wait(communicator, rsnd, rrcv);
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  @tparam T Type of vector
 *  @param[in] communicator  The Communicator object to send over
 *  @param[in] region_sz The size of data to send/receive.
 *  @param[in,out] dat Vector to be accessed
 */
template<typename T>
void send_left(
    const Communicator_mpi& communicator, size_t region_sz, std::vector<T>& dat)
{
    // Check conversions to int are within range
    assert(communicator.get_rank() <= std::numeric_limits<int>::max());
    assert(region_sz <= std::numeric_limits<int>::max());

    int rank = static_cast<int>(communicator.get_rank());
    int cnt  = static_cast<int>(region_sz * sizeof(T));

    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    if (rank != 0) {
        int err = MPI_Isend(
            dat.data(), cnt, MPI_CHAR, rank - 1, 0,
            communicator.mpi_communicator(), &rsnd[0]);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(
                Log_entry{Status::Error,
                          "Sort MPI_Isend error: "s + mpi_error_to_string(err),
                          Verbosity::none,
                          EXSEIS_SOURCE_POSITION("exseis::piol::send_left")});
        }
    }


    assert(communicator.get_num_rank() <= std::numeric_limits<int>::max());

    if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {
        int err = MPI_Irecv(
            &dat[dat.size() - region_sz], cnt, MPI_CHAR, rank + 1, 0,
            communicator.mpi_communicator(), &rrcv[0]);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(
                Log_entry{Status::Error,
                          "Sort MPI_Recv error: "s + mpi_error_to_string(err),
                          Verbosity::none,
                          EXSEIS_SOURCE_POSITION("exseis::piol::send_left")});
        }
    }

    wait(communicator, rrcv, rsnd);
}

/*! Function to sort a given vector by a nearest neighbour approach.
 *  @tparam T Type of vector
 *  @param[in] communicator  The Communicator to sort over
 *  @param[in] region_sz The size of the region which will be shared
 *  @param[in,out] temp1 Temporary vector which is the dat.size()+region_sz.
 *  @param[in,out] dat The vector to sort
 *  @param[in] comp The function to use for less-than comparisons between
 *                  objects in the vector.
 */
template<class T>
void sort(
    const Communicator_mpi& communicator,
    size_t region_sz,
    std::vector<T>& temp1,
    std::vector<T>& dat,
    Compare<T> comp = nullptr)
{
    size_t lnt      = dat.size();
    size_t num_rank = communicator.get_num_rank();
    size_t rank     = communicator.get_rank();
    size_t edge1    = (rank ? region_sz : 0LU);
    size_t edge2    = (rank != num_rank - 1LU ? region_sz : 0LU);

    using Difference = typename std::vector<T>::difference_type;

    // Check conversions ok
    assert(lnt <= std::numeric_limits<Difference>::max());
    assert(edge1 <= std::numeric_limits<Difference>::max());
    assert(edge2 <= std::numeric_limits<Difference>::max());

    if (comp != nullptr) {
        std::sort(
            dat.begin(), std::next(dat.begin(), static_cast<Difference>(lnt)),
            [comp](auto& a, auto& b) -> bool { return comp(a, b); });
    }
    else {
        std::sort(
            dat.begin(), std::next(dat.begin(), static_cast<Difference>(lnt)));
    }

    std::copy(dat.begin(), dat.end(), temp1.begin());
    std::vector<T> temp2;
    // Infinite loop if there is more than one process, otherwise no loop
    while (num_rank > 1) {
        temp2 = temp1;
        send_left(communicator, region_sz, temp1);

        if (comp != nullptr) {
            std::sort(
                std::next(temp1.begin(), static_cast<Difference>(edge1)),
                temp1.end(),
                [comp](auto& a, auto& b) -> bool { return comp(a, b); });
        }
        else {
            // default pair sorting is first then second
            std::sort(
                std::next(temp1.begin(), static_cast<Difference>(edge1)),
                temp1.end());
        }

        send_right(communicator, region_sz, temp1);

        if (comp != nullptr) {
            std::sort(
                temp1.begin(),
                std::prev(temp1.end(), static_cast<Difference>(edge2)),
                [comp](auto& a, auto& b) -> bool { return comp(a, b); });
        }
        else {
            std::sort(
                temp1.begin(),
                std::prev(temp1.end(), static_cast<Difference>(edge2)));
        }

        int reduced = 0;
        for (size_t j = 0; j < lnt && !reduced; j++) {
            reduced += (temp1[j] != temp2[j]);
        }
        int greduced = 1;

        int err = MPI_Allreduce(
            &reduced, &greduced, 1, MPI_INT, MPI_SUM,
            communicator.mpi_communicator());

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(Log_entry{
                Status::Error,
                "Sort MPI_Allreduce error: "s + mpi_error_to_string(err),
                Verbosity::none, EXSEIS_SOURCE_POSITION("exseis::piol::sort")});
        }

        if (!greduced) {
            break;
        }
    }

    for (size_t i = 0; i < lnt; i++) {
        dat[i] = temp1[i];
    }
}

namespace {

/// @brief Get the MPI_Datatype from an Type.
///
/// @tparam Type The Type enum value to convert.
///
/// @returns The equivalent MPI_Datatype
///
template<Type Type>
MPI_Datatype mpi_datatype_from_type()
{
    return mpi_type<typename Native_from_type<Type>::type>();
}

/// @brief Get the MPI_Datatype from an Rule_entry::MdType.
///
/// @param[in] md_type the Rule_entry::MdType to convert.
///
/// @returns The equivalent MPI_Datatype
///
MPI_Datatype mpi_datatype_from_md_type(Rule_entry::MdType md_type)
{

    const auto type = ([md_type] {
        switch (md_type) {
            case Rule_entry::MdType::Long:
                return Type_from_native<Integer>::value;
            case Rule_entry::MdType::Short:
                return Type_from_native<int16_t>::value;
            case Rule_entry::MdType::Float:
                return Type_from_native<Floating_point>::value;
            case Rule_entry::MdType::Index:
                return Type::Index;
            case Rule_entry::MdType::Copy:
                return Type::Copy;
        }
        assert(false && "Unknown MdType!");
    }());

    switch (type) {
        case Type::Double:
            return mpi_datatype_from_type<Type::Double>();
        case Type::Float:
            return mpi_datatype_from_type<Type::Float>();
        case Type::Int64:
            return mpi_datatype_from_type<Type::Int64>();
        case Type::UInt64:
            return mpi_datatype_from_type<Type::UInt64>();
        case Type::Int32:
            return mpi_datatype_from_type<Type::Int32>();
        case Type::UInt32:
            return mpi_datatype_from_type<Type::UInt32>();
        case Type::Int16:
            return mpi_datatype_from_type<Type::Int16>();
        case Type::UInt16:
            return mpi_datatype_from_type<Type::UInt16>();
        case Type::Int8:
            return mpi_datatype_from_type<Type::Int8>();
        case Type::UInt8:
            return mpi_datatype_from_type<Type::UInt8>();
        case Type::Index:
            return mpi_datatype_from_type<Type::UInt64>();
        case Type::Copy:
            return MPI_DATATYPE_NULL;
    }
    assert(false && "Unknown Type!");
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  @param[in]     communicator  The Communicator to sort over
 *  @param[in]     region_sz The size of data to send/receive.
 *  @param[in,out] trace_metadata      The parameter structure to send/receive
 */
void send_right(
    const Communicator_mpi& communicator,
    size_t region_sz,
    Trace_metadata& trace_metadata)
{
    Trace_metadata send_trace_metadata(trace_metadata.rules, region_sz);
    Trace_metadata recv_trace_metadata(trace_metadata.rules, region_sz);

    // Check conversions ok
    assert(
        communicator.get_rank() <= std::numeric_limits<int>::max()
        && "MPI Rank is too large for an int.");

    assert(communicator.get_num_rank() <= std::numeric_limits<int>::max());

    int rank = static_cast<int>(communicator.get_rank());
    std::vector<MPI_Request> rsnd;
    std::vector<MPI_Request> rrcv;

    // Lambda for wrapping an MPI call with logging.
    auto log_on_error = [&communicator](
                            auto mpi_function, std::string function_name,
                            auto&&... args) {
        int err = mpi_function(args...);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(Log_entry{
                Status::Error,
                function_name + " error: "s + mpi_error_to_string(err),
                Verbosity::none,
                EXSEIS_SOURCE_POSITION("exseis::piol::send_right")});
        }
    };

    if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {

        for (size_t i = 0; i < region_sz; i++) {
            send_trace_metadata.copy_entries(
                i, trace_metadata, i + trace_metadata.size() - region_sz);
        }
    }

    for (const auto entry : recv_trace_metadata.entry_types) {
        const auto key     = entry.first;
        const auto md_type = entry.second;

        if (md_type == Rule_entry::MdType::Copy) {
            continue;
        }

        const auto mpi_datatype = mpi_datatype_from_md_type(md_type);

        // Check conversion ok
        assert(
            recv_trace_metadata.entry_size(key)
            < std::numeric_limits<int>::max());

        if (rank != 0) {
            rrcv.push_back(MPI_REQUEST_NULL);

            log_on_error(
                MPI_Irecv, "Sort right Floating_point MPI_Irecv",
                recv_trace_metadata.entry_data<unsigned char>(key),
                static_cast<int>(recv_trace_metadata.entry_size(key)),
                mpi_datatype, rank - 1, 0, communicator.mpi_communicator(),
                &rrcv.back());
        }

        if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {

            rsnd.push_back(MPI_REQUEST_NULL);

            log_on_error(
                MPI_Isend, "Sort right MPI_Isend",
                send_trace_metadata.entry_data<unsigned char>(key),
                static_cast<int>(send_trace_metadata.entry_size(key)),
                mpi_datatype, rank + 1, 0, communicator.mpi_communicator(),
                &rsnd.back());
        }
    }

    wait(communicator, rsnd, rrcv);

    if (rank != 0) {
        for (size_t i = 0; i < region_sz; i++) {
            trace_metadata.copy_entries(i, recv_trace_metadata, i);
        }
    }
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  @param[in]     communicator  The Communicator to send over
 *  @param[in]     region_sz The size of data to send/receive.
 *  @param[in,out] trace_metadata      The parameter structure to send/receive
 */
void send_left(
    const Communicator_mpi& communicator,
    size_t region_sz,
    Trace_metadata& trace_metadata)
{
    Trace_metadata send_trace_metadata(trace_metadata.rules, region_sz);
    Trace_metadata recv_trace_metadata(trace_metadata.rules, region_sz);

    // Check converstions ok
    assert(
        communicator.get_rank() <= std::numeric_limits<int>::max()
        && "PIOL rank too big for MPI");

    assert(
        communicator.get_num_rank() <= std::numeric_limits<int>::max()
        && "PIOL num ranks too big for MPI");

    int rank = static_cast<int>(communicator.get_rank());
    std::vector<MPI_Request> rsnd;
    std::vector<MPI_Request> rrcv;

    // Lambda for wrapping an MPI call with logging.
    auto log_on_error = [&communicator](
                            auto mpi_function, std::string function_name,
                            auto&&... args) {
        int err = mpi_function(args...);

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(Log_entry{
                Status::Error,
                function_name + " error: "s + mpi_error_to_string(err),
                Verbosity::none, EXSEIS_SOURCE_POSITION("send_left")});
        }
    };

    if (rank != 0) {
        for (size_t i = 0; i < region_sz; i++) {
            send_trace_metadata.copy_entries(i, trace_metadata, i);
        }
    }

    // send_trace_metadata and recv_trace_metadata should have the same
    // entry_types.
    for (const auto entry : send_trace_metadata.entry_types) {
        const auto key     = entry.first;
        const auto md_type = entry.second;

        if (md_type == Rule_entry::MdType::Copy) {
            continue;
        }

        const auto mpi_datatype = mpi_datatype_from_md_type(md_type);

        // Check conversion ok
        assert(
            send_trace_metadata.entry_size(key)
            < std::numeric_limits<int>::max());

        if (rank != 0) {
            rsnd.push_back(MPI_REQUEST_NULL);

            log_on_error(
                MPI_Isend, "Sort left MPI_Isend",
                send_trace_metadata.entry_data<unsigned char>(key),
                static_cast<int>(send_trace_metadata.entry_size(key)),
                mpi_datatype, rank - 1, 1, communicator.mpi_communicator(),
                &rsnd.back());
        }

        if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {
            rrcv.push_back(MPI_REQUEST_NULL);

            log_on_error(
                MPI_Irecv, "Sort left MPI_Irecv",
                recv_trace_metadata.entry_data<unsigned char>(key),
                static_cast<int>(recv_trace_metadata.entry_size(key)),
                mpi_datatype, rank + 1, 1, communicator.mpi_communicator(),
                &rrcv.back());
        }
    }

    wait(communicator, rrcv, rsnd);

    if (rank != static_cast<int>(communicator.get_num_rank()) - 1) {
        for (size_t i = 0; i < region_sz; i++) {
            trace_metadata.copy_entries(
                i + trace_metadata.size() - region_sz, recv_trace_metadata, i);
        }
    }
}


/// Sort the parameter structure across all processes
/// @param[in] communicator  The Communicator to sort over
/// @param[in] trace_metadata  The parameter structure
/// @param[in] comp            The comparison operator to sort the headers by.
void sort_p(
    const Communicator_mpi& communicator,
    Trace_metadata& trace_metadata,
    CompareP comp = nullptr)
{
    size_t lnt       = trace_metadata.size();
    size_t num_rank  = communicator.get_num_rank();
    size_t rank      = communicator.get_rank();
    size_t region_sz = communicator.min(lnt) / 4LU;
    size_t edge1     = (rank != 0 ? region_sz : 0LU);
    size_t edge2     = (rank != num_rank - 1 ? region_sz : 0LU);

    Trace_metadata temp1(trace_metadata.rules, lnt + edge2);
    Trace_metadata temp2(temp1.rules, temp1.size());
    Trace_metadata temp3(trace_metadata.rules, temp1.size());

    {
        std::vector<size_t> t1(lnt);
        std::iota(t1.begin(), t1.end(), 0LU);
        std::sort(t1.begin(), t1.end(), [&](size_t& a, size_t& b) -> bool {
            return comp(trace_metadata, a, b);
        });

        for (size_t i = 0; i < lnt; i++) {
            temp1.copy_entries(i, trace_metadata, t1[i]);
        }
    }

    // Infinite loop if there is more than one process, otherwise no loop
    while (num_rank > 1) {
        temp2 = temp1;
        send_left(communicator, region_sz, temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge1);
            std::iota(t1.begin(), t1.end(), edge1);
            std::sort(
                t1.begin(), t1.end(),
                [&temp1, comp](size_t& a, size_t& b) -> bool {
                    return comp(temp1, a, b);
                });

            for (size_t i = 0; i < t1.size(); i++) {
                temp3.copy_entries(i, temp1, t1[i]);
            }

            for (size_t i = 0; i < t1.size(); i++) {
                temp1.copy_entries(i + edge1, temp3, i);
            }
        }

        send_right(communicator, region_sz, temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge2);
            std::iota(t1.begin(), t1.end(), 0);
            std::sort(
                t1.begin(), t1.end(),
                [&temp1, comp](size_t& a, size_t& b) -> bool {
                    return comp(temp1, a, b);
                });

            for (size_t i = 0; i < t1.size(); i++) {
                temp3.copy_entries(i, temp1, t1[i]);
            }

            for (size_t i = 0; i < t1.size(); i++) {
                temp1.copy_entries(i, temp3, i);
            }
        }

        int reduced = 0;
        for (size_t j = 0; j < lnt && reduced == 0; j++) {
            const auto param1 = temp1.get_index(j, Trace_metadata_key::gtn);

            const auto param2 = temp2.get_index(j, Trace_metadata_key::gtn);

            if (param1 != param2) {
                reduced++;
            }
        }
        int greduced = 1;

        int err = MPI_Allreduce(
            &reduced, &greduced, 1, MPI_INT, MPI_SUM,
            communicator.mpi_communicator());

        if (err != MPI_SUCCESS) {
            communicator.log()->add_entry(Log_entry{
                Status::Error,
                "Sort MPI_Allreduce error: " + mpi_error_to_string(err),
                Verbosity::none,
                EXSEIS_SOURCE_POSITION("exseis::piol::sortP")});
        }

        if (greduced == 0) {
            break;
        }
    }

    for (size_t i = 0; i < lnt; i++) {
        trace_metadata.copy_entries(i, temp1, i);
    }
}


}  // namespace


std::vector<size_t> sort(
    const Communicator_mpi& communicator,
    Trace_metadata& trace_metadata,
    CompareP comp,
    bool file_order)
{
    sort_p(communicator, trace_metadata, comp);

    std::vector<size_t> list(trace_metadata.size());
    for (size_t i = 0; i < trace_metadata.size(); i++) {
        list[i] = trace_metadata.get_index(i, Trace_metadata_key::gtn);
    }


    if (!file_order) {
        return list;
    }


    const size_t lnt       = list.size();
    const size_t region_sz = communicator.min(lnt) / 4LU;
    const size_t edge2 =
        (communicator.get_rank() != communicator.get_num_rank() - 1 ?
             region_sz :
             0LU);
    const size_t offset = communicator.offset(lnt);

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++) {
        plist[i].first  = list[i];
        plist[i].second = offset + i;
    }

    {
        std::vector<std::pair<size_t, size_t>> temp1(lnt + edge2);

// Work-around for bug in the intel compiler (intel/2016-u3) on Fionn with
// std::pair less-than operator
#if defined(__INTEL_COMPILER)
        Compare<std::pair<size_t, size_t>> check =
            [](const std::pair<size_t, size_t>& e1,
               const std::pair<size_t, size_t>& e2) -> bool {
            return (
                e1.first < e2.first
                || (e1.first == e2.first && e1.second < e2.second));
        };
        sort(communicator, region_sz, temp1, plist, check);
#else
        sort(communicator, region_sz, temp1, plist);
#endif
    }

    for (size_t i = 0; i < lnt; i++) {
        list[i] = plist[i].second;
    }

    return list;
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis
