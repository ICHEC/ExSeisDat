/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date May 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "share/decomp.hh"
#include "ops/gather.hh"
#include "file/dynsegymd.hh"
namespace PIOL { namespace File {
/*! Find the inline/crossline for each il/xl gather and the number of traces per gather
 *  based on the parameters provided and assuming this is collectively called.
 * \param[in] piol The piol object.
 * \param[in] prm The parameter structure.
 * \return Return an 'array' of tuples. Each tuple corresponds to each gather. Tuple elements:
 *         1) Number of traces in the gather, 2) inline, 3) crossline.
 * \todo TODO: This can be generalised
 */
Uniray<size_t, llint, llint> getGathers(ExSeisPIOL * piol, Param * prm)
{
    size_t rank = piol->comm->getRank();
    size_t last = rank == piol->comm->getNumRank()-1;
    std::vector<std::tuple<size_t, llint, llint>> lline;
    lline.emplace_back(0LU, getPrm<llint>(0, Meta::il, prm), getPrm<llint>(0, Meta::xl, prm));
    ++std::get<0>(lline.back());

    for (size_t i = 1; i < prm->size()-(last ? 0LU : 1LU); i++)
    {
        llint il = getPrm<llint>(i, Meta::il, prm);
        llint xl = getPrm<llint>(i, Meta::xl, prm);

        if (il != getPrm<llint>(i-1LU, Meta::il, prm) ||
            xl != getPrm<llint>(i-1LU, Meta::xl, prm))
            lline.emplace_back(0LU, il, xl);
        ++std::get<0>(lline.back());
    }

    //If the last element is on the same gather, then the gather has already been picked up
    //by the process one rank higher.
    size_t gatherB = !last
           && getPrm<llint>(prm->size()-1LU, Meta::il, prm) == getPrm<llint>(prm->size()-2LU, Meta::il, prm)
           && getPrm<llint>(prm->size()-1LU, Meta::xl, prm) == getPrm<llint>(prm->size()-2LU, Meta::xl, prm);

    size_t lSz = lline.size() - gatherB;
    size_t offset = piol->comm->offset(lSz);

    //Nearest neighbour pass would be more appropriate
    auto left = piol->comm->gather<size_t>(gatherB ? std::get<0>(lline.back()) : 0LU);
    std::get<0>(lline.front()) += (rank ? left[rank-1] : 0LU);

    Uniray<size_t, llint, llint> line(piol, piol->comm->sum(lSz));
    for (size_t i = 0; i < lSz; i++)
        line.set(i + offset, lline[i]);

//TODO: Check if a gather extends over three processes and compensate accordingly

    piol->comm->barrier();
    return line;
}

Uniray<size_t, llint, llint> getIlXlGathers(ExSeisPIOL * piol, ReadInterface * file)
{
    auto dec = decompose(piol, file);
    dec.second += (piol->comm->getRank() < piol->comm->getNumRank()-1LU);   //Each process should read an overlapping entry, except last

    auto rule = std::make_shared<Rule>(std::initializer_list<Meta>{Meta::il, Meta::xl});
    Param prm(rule, dec.second);
    file->readParam(dec.first, dec.second, &prm);

    return getGathers(piol, &prm);
}
}}
