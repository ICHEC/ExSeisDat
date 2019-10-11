////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date May 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector_mpi.hh"

namespace exseis {
namespace piol {
inline namespace operations {

/*! Find the inline/crossline for each il/xl gather and the number of traces per
 *  gather based on the parameters provided and assuming this is collectively
 *  called.
 * @param[in] piol The piol object.
 * @param[in] prm The parameter structure.
 * @return Return an 'array' of tuples. Each tuple corresponds to each gather.
 *         Tuple elements: 1) Number of traces in the gather, 2) inline, 3)
 *         crossline.
 * @todo TODO: This can be generalised
 */
static utils::Distributed_vector<Gather_info> get_gathers(
    ExSeisPIOL* piol, Trace_metadata* prm)
{
    size_t rank     = piol->comm->get_rank();
    size_t num_rank = piol->comm->get_num_rank();
    std::vector<Gather_info> lline;

    exseis::utils::Integer ill = prm->get_integer(0LU, Trace_metadata_key::il);
    exseis::utils::Integer xll = prm->get_integer(0LU, Trace_metadata_key::xl);
    lline.push_back({1LU, ill, xll});

    for (size_t i = 1; i < prm->size(); i++) {
        exseis::utils::Integer il = prm->get_integer(i, Trace_metadata_key::il);
        exseis::utils::Integer xl = prm->get_integer(i, Trace_metadata_key::xl);

        if (il != ill || xl != xll) {
            lline.push_back({0LU, il, xl});
            ill = il;
            xll = xl;
        }
        lline.back().num_traces += 1;
    }

    auto trcnum = piol->comm->gather(lline.front().num_traces);
    auto ilb    = piol->comm->gather(lline.back().in_line);
    auto xlb    = piol->comm->gather(lline.back().crossline);
    auto ilf    = piol->comm->gather(lline.front().in_line);
    auto xlf    = piol->comm->gather(lline.front().crossline);

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
    size_t offset = piol->comm->offset(sz);

    utils::Distributed_vector_mpi<Gather_info> line(
        piol->comm->sum(sz), piol->comm->get_comm());

    for (size_t i = 0; i < sz; i++) {
        line.set(i + offset, lline[i + start]);
    }

    return std::move(line);
}

utils::Distributed_vector<Gather_info> get_il_xl_gathers(
    ExSeisPIOL* piol, Input_file* file)
{
    auto dec = utils::block_decomposition(
        file->read_nt(), piol->comm->get_num_rank(), piol->comm->get_rank());

    const auto rule = Rule(std::initializer_list<Trace_metadata_key>{
        Trace_metadata_key::il, Trace_metadata_key::xl});

    Trace_metadata prm(std::move(rule), dec.local_size);

    file->read_param(dec.global_offset, dec.local_size, &prm);

    return get_gathers(piol, &prm);
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis
