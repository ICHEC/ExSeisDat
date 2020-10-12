////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date May 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/operations/gather.hh"
#include "exseis/utils/communicator/Communicator.hh"
#include "exseis/utils/decomposition/block_decomposition.hh"
#include "exseis/utils/distributed_vector/Distributed_vector_mpi.hh"

namespace exseis {
inline namespace piol {
inline namespace operations {

/*! Find the inline/crossline for each il/xl gather and the number of traces per
 *  gather based on the parameters provided and assuming this is collectively
 *  called.
 * @param[in] piol The piol object.
 * @param[in] trace_metadata The parameter structure.
 * @return Return an 'array' of tuples. Each tuple corresponds to each gather.
 *         Tuple elements: 1) Number of traces in the gather, 2) inline, 3)
 *         crossline.
 * @todo TODO: This can be generalised
 */
static void get_gathers(
    const Communicator& communicator,
    Trace_metadata* trace_metadata,
    Distributed_vector<Gather_info>& lines)
{
    size_t rank     = communicator.get_rank();
    size_t num_rank = communicator.get_num_rank();
    std::vector<Gather_info> lline;

    Integer ill = trace_metadata->get_integer(0LU, Trace_metadata_key::il);
    Integer xll = trace_metadata->get_integer(0LU, Trace_metadata_key::xl);
    lline.push_back({1LU, ill, xll});

    for (size_t i = 1; i < trace_metadata->size(); i++) {
        Integer il = trace_metadata->get_integer(i, Trace_metadata_key::il);
        Integer xl = trace_metadata->get_integer(i, Trace_metadata_key::xl);

        if (il != ill || xl != xll) {
            lline.push_back({0LU, il, xl});
            ill = il;
            xll = xl;
        }
        lline.back().num_traces += 1;
    }

    auto trcnum = communicator.gather(lline.front().num_traces);
    auto ilb    = communicator.gather(lline.back().in_line);
    auto xlb    = communicator.gather(lline.back().crossline);
    auto ilf    = communicator.gather(lline.front().in_line);
    auto xlf    = communicator.gather(lline.front().crossline);

    size_t start = 0;
    if (rank != 0 && ilb[rank - 1LU] == ilf[rank]
        && xlb[rank - 1LU] == xlf[rank]) {

        start = 1;
    }

    if (start < lline.size()) {
        if (ilf[rank + 1LU] == ilb[rank] && xlf[rank + 1LU] == xlb[rank]) {
            for (size_t i = rank + 1LU; i < num_rank; i++) {
                if (ilb[rank] == ilf[i] && xlb[rank] == xlf[i]) {
                    lline.back().num_traces += trcnum[i];
                }
                else {
                    break;
                }
            }
        }
    }

    size_t sz     = lline.size() - start;
    size_t offset = communicator.offset(sz);

    lines.resize(communicator.sum(sz));

    for (size_t i = 0; i < sz; i++) {
        lines.set(i + offset, lline[i + start]);
    }
}

void get_il_xl_gathers(
    const Communicator& communicator,
    const Input_file& file,
    Distributed_vector<Gather_info>& lines)
{
    auto dec = block_decomposition(
        file.read_number_of_traces(), communicator.get_num_rank(),
        communicator.get_rank());

    const auto rule = Rule(std::initializer_list<Trace_metadata_key>{
        Trace_metadata_key::il, Trace_metadata_key::xl});

    Trace_metadata trace_metadata(std::move(rule), dec.local_size);

    file.read_metadata(dec.global_offset, dec.local_size, trace_metadata);

    return get_gathers(communicator, &trace_metadata, lines);
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis
