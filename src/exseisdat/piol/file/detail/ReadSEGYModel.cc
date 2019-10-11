////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of \c ReadSEGYModel
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/file/detail/ReadSEGYModel.hh"
#include "exseisdat/piol/metadata/Trace_metadata.hh"

namespace exseis {
namespace piol {
inline namespace file {
inline namespace detail {

ReadSEGYModel::ReadSEGYModel(
    std::shared_ptr<ExSeisPIOL> piol,
    const std::string name,
    const ReadSEGYModel::Options& opt) :
    Input_file_segy(piol, name, opt)
{
    std::vector<size_t> vlist = {0LU, 1LU, Input_file_segy::read_nt() - 1LU};
    Trace_metadata prm(vlist.size());
    read_param_non_contiguous(vlist.size(), vlist.data(), &prm);

    exseis::utils::Integer il_start =
        prm.get_integer(0LU, Trace_metadata_key::il);
    exseis::utils::Integer xl_start =
        prm.get_integer(0LU, Trace_metadata_key::xl);


    exseis::utils::Integer il_increment =
        prm.get_integer(1LU, Trace_metadata_key::il) - il_start;

    exseis::utils::Integer il_count =
        (il_increment != 0 ?
             (prm.get_integer(2LU, Trace_metadata_key::il) - il_start)
                 / il_increment :
             0);

    assert(
        Input_file_segy::read_nt()
        < std::numeric_limits<exseis::utils::Integer>::max());
    exseis::utils::Integer xl_count =
        (static_cast<exseis::utils::Integer>(Input_file_segy::read_nt())
         / (il_count != 0 ? il_count : 1));

    exseis::utils::Integer xl_increment =
        (prm.get_integer(2LU, Trace_metadata_key::xl) - xl_start) / xl_count;

    il_increment = (il_increment != 0 ? il_increment : 1LU);
    xl_increment = (xl_increment != 0 ? xl_increment : 1LU);

    il = CoordinateParameters(il_start, il_count, il_increment);
    xl = CoordinateParameters(xl_start, xl_count, xl_increment);
}

std::vector<exseis::utils::Trace_value> ReadSEGYModel::read_model(
    const size_t offset,
    const size_t sz,
    const utils::Distributed_vector<Gather_info>& gather)
{
    std::vector<exseis::utils::Trace_value> trc(sz * read_ns());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val = gather[offset + i];

        // Check numeric conversions are ok
        assert(il.start <= std::numeric_limits<exseis::utils::Integer>::max());
        assert(xl.start <= std::numeric_limits<exseis::utils::Integer>::max());

        // The below can be translated to:
        //     trace number = il_number * xlInc + xl_number
        // much like indexing in a 2d array.
        //
        offsets[i] = static_cast<size_t>(
            ((val.in_line - il.start) / il.increment) * xl.count
            + ((val.crossline - xl.start) / xl.increment));
    }

    read_trace_non_contiguous(
        offsets.size(), offsets.data(), trc.data(), nullptr, 0LU);

    return trc;
}

std::vector<exseis::utils::Trace_value> ReadSEGYModel::read_model(
    const size_t sz,
    const size_t* offset,
    const utils::Distributed_vector<Gather_info>& gather)
{
    std::vector<exseis::utils::Trace_value> trc(sz * read_ns());
    std::vector<size_t> offsets(sz);
    for (size_t i = 0; i < sz; i++) {
        auto val   = gather[offset[i]];
        offsets[i] = static_cast<size_t>(
            ((val.in_line - il.start) / il.increment) * xl.count
            + ((val.crossline - xl.start) / xl.increment));
    }

    read_trace_non_contiguous(
        offsets.size(), offsets.data(), trc.data(), nullptr, 0LU);

    return trc;
}

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis
