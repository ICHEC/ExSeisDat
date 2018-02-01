#include "cppfileapi.hh"
#include "ops/minmax.hh"
#include "sglobal.hh"

#include <algorithm>
#include <iostream>

using namespace PIOL;
using namespace File;

/*! Read from the input file. Find the min/max  xSrc, ySrc, xRcv, yRcv, xCmp
 *  and yCMP. Write the matching traces to the output file in that order.
 *  @param[in] iname Input file
 *  @param[in] oname Output file
 */
void calcMin(std::string iname, std::string oname)
{
    auto piol = ExSeis::New();
    File::ReadDirect in(piol, iname);

    auto dec      = decompose(piol.get(), in);
    size_t offset = dec.first;
    size_t lnt    = dec.second;

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
                cpyPrm(j, &tprm, i, &oprm);
                setPrm(i, PIOL_META_tn, minmax[i].num, &oprm);
                trace[i] = trace_t(1);
                j        = usz;
            }
        }

    File::WriteDirect out(piol, oname);
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
