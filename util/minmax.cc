#include "sglobal.hh"

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/WriteDirect.hh"
#include "ExSeisDat/PIOL/operations/minmax.h"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/utils/decomposition/block_decomposition.h"

#include <algorithm>
#include <iostream>

using namespace exseis::utils;
using namespace exseis::PIOL;

/*! Read from the input file. Find the min/max  xSrc, ySrc, xRcv, yRcv, xCmp
 *  and yCMP. Write the matching traces to the output file in that order.
 *  @param[in] iname Input file
 *  @param[in] oname Output file
 */
void calcMin(std::string iname, std::string oname)
{
    auto piol = ExSeis::New();
    ReadDirect in(piol, iname);

    auto dec = block_decomposition(
      in.readNt(), piol->comm->getNumRank(), piol->comm->getRank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    Param prm(lnt);
    std::vector<CoordElem> minmax(12U);
    in.readParam(offset, lnt, &prm);

    getMinMax(
      piol.get(), offset, lnt, PIOL_META_xSrc, PIOL_META_ySrc, &prm,
      minmax.data());
    getMinMax(
      piol.get(), offset, lnt, PIOL_META_xRcv, PIOL_META_yRcv, &prm,
      minmax.data() + 4U);
    getMinMax(
      piol.get(), offset, lnt, PIOL_META_xCmp, PIOL_META_yCmp, &prm,
      minmax.data() + 8U);

    size_t sz  = (!piol->getRank() ? minmax.size() : 0U);
    size_t usz = 0;
    std::vector<size_t> list(sz);
    std::vector<size_t> uniqlist(sz);

    if (sz) {
        for (size_t i = 0U; i < sz; i++)
            uniqlist[i] = list[i] = minmax[i].num;

        std::sort(uniqlist.begin(), uniqlist.end());
        auto end = std::unique(uniqlist.begin(), uniqlist.end());
        uniqlist.resize(std::distance(uniqlist.begin(), end));

        usz = uniqlist.size();
    }

    Param tprm(usz);
    in.readParamNonContiguous(usz, uniqlist.data(), &tprm);

    Param oprm(sz);
    std::vector<trace_t> trace(sz);
    for (size_t i = 0U; i < sz; i++)
        for (size_t j = 0U; j < usz; j++) {
            if (list[i] == uniqlist[j]) {
                param_utils::cpyPrm(j, &tprm, i, &oprm);
                param_utils::setPrm(i, PIOL_META_tn, minmax[i].num, &oprm);
                trace[i] = trace_t(1);
                j        = usz;
            }
        }

    WriteDirect out(piol, oname);
    out.writeNt(sz);
    out.writeNs(1U);
    out.writeInc(in.readInc());
    out.writeTrace(0, sz, trace.data(), &oprm);
}

/* Main function for minmax.
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details Options:
 *           -i \<file\> : input file name
 *           -o \<file\> : output file name
 */
int main(int argc, char** argv)
{
    std::string iname = "";
    std::string oname = "";

    std::string opt = "i:o:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str()))
        switch (c) {
            case 'i':
                iname = optarg;
                break;
            case 'o':
                oname = optarg;
                break;
            default:
                std::cerr << "One of the command line arguments is invalid.\n";
                return -1;
        }

    if (iname == "" || oname == "") {
        std::cerr << "Invalid arguments given.\n";
        std::cerr << "Arguments: -i for input file, -o for output file\n";
        return -1;
    }

    calcMin(iname, oname);

    return 0;
}
