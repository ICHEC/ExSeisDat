#include "sglobal.hh"

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/WriteSEGY.hh"
#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"

#include <algorithm>
#include <iostream>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::piol;

/*! Read from the input file. Find the min/max  x_src, y_src, x_rcv, y_rcv, xCmp
 *  and yCMP. Write the matching traces to the output file in that order.
 *  @param[in] iname Input file
 *  @param[in] oname Output file
 */
void calc_min(std::string iname, std::string oname)
{
    auto piol = ExSeis::make();
    ReadSEGY in(piol, iname);

    auto dec = block_decomposition(
      in.read_nt(), piol->comm->get_num_rank(), piol->comm->get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    Trace_metadata prm(lnt);
    std::vector<CoordElem> minmax(12U);
    in.read_param(offset, lnt, &prm);

    get_min_max(
      piol.get(), offset, lnt, Meta::x_src, Meta::y_src, prm, minmax.data());
    get_min_max(
      piol.get(), offset, lnt, Meta::x_rcv, Meta::y_rcv, prm,
      minmax.data() + 4U);
    get_min_max(
      piol.get(), offset, lnt, Meta::xCmp, Meta::yCmp, prm, minmax.data() + 8U);

    size_t sz  = (piol->get_rank() == 0 ? minmax.size() : 0U);
    size_t usz = 0;
    std::vector<size_t> list(sz);
    std::vector<size_t> uniqlist(sz);

    if (sz != 0) {
        for (size_t i = 0U; i < sz; i++) {
            uniqlist[i] = list[i] = minmax[i].num;
        }

        std::sort(uniqlist.begin(), uniqlist.end());
        auto end = std::unique(uniqlist.begin(), uniqlist.end());
        uniqlist.resize(std::distance(uniqlist.begin(), end));

        usz = uniqlist.size();
    }

    Trace_metadata tprm(usz);
    in.read_param_non_contiguous(usz, uniqlist.data(), &tprm);

    Trace_metadata oprm(sz);
    std::vector<exseis::utils::Trace_value> trace(sz);
    for (size_t i = 0U; i < sz; i++) {
        for (size_t j = 0U; j < usz; j++) {
            if (list[i] == uniqlist[j]) {
                oprm.copy_entries(i, tprm, j);
                oprm.set_integer(i, Meta::tn, minmax[i].num);
                trace[i] = exseis::utils::Trace_value(1);
                j        = usz;
            }
        }
    }

    WriteSEGY out(piol, oname);
    out.write_nt(sz);
    out.write_ns(1U);
    out.write_sample_interval(in.read_sample_interval());
    out.write_trace(0, sz, trace.data(), &oprm);
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
    std::string iname;
    std::string oname;

    std::string opt = "i:o:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
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
    }

    if (iname.empty() || oname.empty()) {
        std::cerr << "Invalid arguments given.\n";
        std::cerr << "Arguments: -i for input file, -o for output file\n";
        return -1;
    }

    calc_min(iname, oname);

    return 0;
}
