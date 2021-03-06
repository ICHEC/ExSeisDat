////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date May 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/operations/gather.hh"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/utils/decomposition/block_decomposition.h"
#include "ExSeisDat/utils/mpi/MPI_Distributed_vector.hh"

namespace exseis {
namespace PIOL {

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
static utils::Distributed_vector<Gather_info> getGathers(
  ExSeisPIOL* piol, Param* prm)
{
    size_t rank    = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    std::vector<Gather_info> lline;

    exseis::utils::Integer ill =
      param_utils::getPrm<exseis::utils::Integer>(0LU, PIOL_META_il, prm);
    exseis::utils::Integer xll =
      param_utils::getPrm<exseis::utils::Integer>(0LU, PIOL_META_xl, prm);
    lline.push_back({1LU, ill, xll});

    for (size_t i = 1; i < prm->size(); i++) {
        exseis::utils::Integer il =
          param_utils::getPrm<exseis::utils::Integer>(i, PIOL_META_il, prm);
        exseis::utils::Integer xl =
          param_utils::getPrm<exseis::utils::Integer>(i, PIOL_META_xl, prm);

        if (il != ill || xl != xll) {
            lline.push_back({0LU, il, xl});
            ill = il;
            xll = xl;
        }
        lline.back().num_traces += 1;
    }

    auto trcnum = piol->comm->gather<size_t>(lline.front().num_traces);
    auto ilb    = piol->comm->gather<size_t>(lline.back().inline_);
    auto xlb    = piol->comm->gather<size_t>(lline.back().crossline);
    auto ilf    = piol->comm->gather<size_t>(lline.front().inline_);
    auto xlf    = piol->comm->gather<size_t>(lline.front().crossline);

    size_t start = 0;
    if (
      rank != 0 && ilb[rank - 1LU] == ilf[rank]
      && xlb[rank - 1LU] == xlf[rank]) {

        start = 1;
    }

    if (start < lline.size()) {
        if (ilf[rank + 1LU] == ilb[rank] && xlf[rank + 1LU] == xlb[rank]) {
            for (size_t i = rank + 1LU; i < numRank; i++) {
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

    utils::MPI_Distributed_vector<Gather_info> line(
      piol->comm->sum(sz), piol->comm->getComm());

    for (size_t i = 0; i < sz; i++) {
        line.set(i + offset, lline[i + start]);
    }

    return std::move(line);
}

utils::Distributed_vector<Gather_info> getIlXlGathers(
  ExSeisPIOL* piol, ReadInterface* file)
{
    auto dec = utils::block_decomposition(
      file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());

    auto rule = std::make_shared<Rule>(
      std::initializer_list<Meta>{PIOL_META_il, PIOL_META_xl});

    Param prm(rule, dec.local_size);

    file->readParam(dec.global_offset, dec.local_size, &prm);

    return getGathers(piol, &prm);
}

}  // namespace PIOL
}  // namespace exseis
