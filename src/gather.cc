/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @date May 2017
 *   @brief
 *   @details
 *//*******************************************************************************************/
#include "share/decomp.hh"
#include "ops/gather.hh"
#include "file/dynsegymd.hh"
namespace PIOL { namespace File {
/*! Find the inline/crossline for each il/xl gather and the number of traces per gather
 *  based on the parameters provided and assuming this is collectively called.
 * @param[in] piol The piol object.
 * @param[in] prm The parameter structure.
 * @return Return an 'array' of tuples. Each tuple corresponds to each gather. Tuple elements:
 *         1) Number of traces in the gather, 2) inline, 3) crossline.
 * @todo TODO: This can be generalised
 */
Uniray<size_t, llint, llint> getGathers(ExSeisPIOL * piol, Param * prm)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    std::vector<std::tuple<size_t, llint, llint>> lline;

    llint ill = getPrm<llint>(0LU, PIOL_META_il, prm);
    llint xll = getPrm<llint>(0LU, PIOL_META_xl, prm);
    lline.emplace_back(1LU, ill, xll);

    for (size_t i = 1; i < prm->size(); i++)
    {
        llint il = getPrm<llint>(i, PIOL_META_il, prm);
        llint xl = getPrm<llint>(i, PIOL_META_xl, prm);

        if (il != ill || xl != xll)
        {
            lline.emplace_back(0LU, il, xl);
            ill = il;
            xll = xl;
        }
        ++std::get<0>(lline.back());
    }

    auto trcnum = piol->comm->gather<size_t>(std::get<0>(lline.front()));
    auto ilb = piol->comm->gather<size_t>(std::get<1>(lline.back()));
    auto xlb = piol->comm->gather<size_t>(std::get<2>(lline.back()));
    auto ilf = piol->comm->gather<size_t>(std::get<1>(lline.front()));
    auto xlf = piol->comm->gather<size_t>(std::get<2>(lline.front()));

    size_t start = (rank ? ilb[rank-1LU] == ilf[rank] && xlb[rank-1LU] == xlf[rank] : 0U);
    if (start < lline.size())
        if (ilf[rank+1LU] == ilb[rank] && xlf[rank+1LU] == xlb[rank])
            for (size_t i = rank+1LU; i < numRank && ilb[rank] == ilf[i] && xlb[rank] == xlf[i]; i++)
                std::get<0>(lline.back()) += trcnum[i];

    size_t sz = lline.size()-start;
    size_t offset = piol->comm->offset(sz);

    Uniray<size_t, llint, llint> line(piol, piol->comm->sum(sz));
    for (size_t i = 0; i < sz; i++)
        line.set(i + offset, lline[i+start]);

    return line;
}

Uniray<size_t, llint, llint> getIlXlGathers(ExSeisPIOL * piol, ReadInterface * file)
{
    auto dec = decompose(piol, file);
    auto rule = std::make_shared<Rule>(std::initializer_list<Meta>{PIOL_META_il, PIOL_META_xl});
    Param prm(rule, dec.second);
    file->readParam(dec.first, dec.second, &prm);

    return getGathers(piol, &prm);
}
}}
