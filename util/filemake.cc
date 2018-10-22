#include "sglobal.hh"

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/WriteSEGY.hh"
#include "exseisdat/piol/segy/utils.hh"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::piol;

void write_contig(
  ExSeis& piol,
  WriteInterface& file,
  size_t offset,
  size_t nt,
  size_t ns,
  size_t lnt,
  size_t extra,
  size_t max)
{
    float fhalf = float(nt * ns) / 2.0;
    float off   = float(nt * ns) / 4.0;
    long nhalf  = nt / 2;
    Trace_metadata prm(max);
    std::vector<float> trc(max * ns);
    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        for (size_t j = 0; j < rblock; j++) {
            float k = nhalf - std::abs(-nhalf + long(offset + i + j));
            prm.set_floating_point(j, Meta::x_src, 1600.0 + k);
            prm.set_floating_point(j, Meta::y_src, 2400.0 + k);
            prm.set_floating_point(j, Meta::x_rcv, 100000.0 + k);
            prm.set_floating_point(j, Meta::y_rcv, 3000000.0 + k);
            prm.set_floating_point(j, Meta::xCmp, 10000.0 + k);
            prm.set_floating_point(j, Meta::yCmp, 4000.0 + k);
            prm.set_integer(j, Meta::il, 2400 + k);
            prm.set_integer(j, Meta::xl, 1600 + k);
            prm.set_integer(j, Meta::tn, offset + i + j);
        }
        for (size_t j = 0; j < trc.size(); j++) {
            trc[j] =
              fhalf - std::abs(-fhalf + float((offset + i) * ns + j)) - off;
        }
        file.write_trace(offset + i, rblock, trc.data(), &prm);
        piol.assert_ok();
    }
    for (size_t j = 0; j < extra; j++) {
        file.write_trace(0U, size_t(0), nullptr, nullptr);
        piol.assert_ok();
    }
}

void write_random(
  ExSeis& piol,
  WriteInterface& file,
  size_t nt,
  size_t ns,
  size_t lnt,
  size_t extra,
  size_t max)
{
    float fhalf = float(nt * ns) / 2.0;
    float off   = float(nt * ns) / 4.0;
    long nhalf  = nt / 2;

    std::vector<size_t> offset(lnt);
    auto num = piol.comm->gather<size_t>(lnt);

    size_t rank     = piol.comm->get_rank();
    size_t num_rank = piol.comm->get_num_rank();

    size_t offcount = 0;
    size_t t        = 0;

    // Work out offsets
    while (num[rank] > 0 && offcount < nt) {
        for (size_t j = 0; j < num_rank; j++) {
            if (j != rank) {
                if (num[j] != 0) {
                    num[j]--;
                    offcount++;
                }
            }
            else {
                num[rank]--;
                offset[t++] = offcount++;
            }
        }
    }

    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        Trace_metadata prm(rblock);
        std::vector<float> trc(rblock * ns);

        for (size_t j = 0; j < rblock; j++) {
            float k = nhalf - std::abs(-nhalf + long(offset[i + j]));
            prm.set_floating_point(j, Meta::x_src, 1600.0 + k);
            prm.set_floating_point(j, Meta::y_src, 2400.0 + k);
            prm.set_floating_point(j, Meta::x_rcv, 100000.0 + k);
            prm.set_floating_point(j, Meta::y_rcv, 3000000.0 + k);
            prm.set_floating_point(j, Meta::xCmp, 10000.0 + k);
            prm.set_floating_point(j, Meta::yCmp, 4000.0 + k);
            prm.set_integer(j, Meta::il, 2400 + k);
            prm.set_integer(j, Meta::xl, 1600 + k);
            prm.set_integer(j, Meta::tn, offset[i + j]);
        }
        for (size_t j = 0; j < trc.size(); j++) {
            trc[j] =
              fhalf - std::abs(-fhalf + float((offset[i]) * ns + j)) - off;
        }
        file.write_trace_non_contiguous(rblock, &offset[i], trc.data(), &prm);
        piol.assert_ok();
    }

    for (size_t j = 0; j < extra; j++) {
        file.write_trace_non_contiguous(0U, nullptr, nullptr, nullptr);
        piol.assert_ok();
    }
}

void file_make(
  bool lob,
  bool random,
  const std::string name,
  size_t max,
  size_t ns,
  size_t nt,
  exseis::utils::Floating_point sample_interval)
{
    auto piol = ExSeis::make();

    WriteSEGY file(piol, name);

    piol->assert_ok();
    file.write_ns(ns);
    file.write_nt(nt);
    file.write_sample_interval(sample_interval);
    file.write_text("Test file\n");
    piol->assert_ok();

    size_t offset  = 0;
    size_t lnt     = 0;
    size_t biggest = 0;

    if (lob) {
        auto dec =
          lobdecompose(piol.get(), nt, piol->get_num_rank(), piol->get_rank());
        offset  = dec[0];
        lnt     = dec[1];
        biggest = dec[2];
    }
    else {
        auto dec =
          block_decomposition(nt, piol->get_num_rank(), piol->get_rank());

        offset  = dec.global_offset;
        lnt     = dec.local_size;
        biggest = piol->comm->max(lnt);
    }

    // TODO: Add memusage for Trace_metadata
    max /=
      (segy::segy_trace_size(ns) + segy::segy_trace_data_size(ns)
       + sizeof(size_t));
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));
    if (random) {
        write_random(*piol, file, nt, ns, lnt, extra, max);
    }
    else {
        write_contig(*piol, file, offset, nt, ns, lnt, extra, max);
    }
}

int main(int argc, char** argv)
{
    std::string name;
    size_t ns                                     = 0;
    size_t nt                                     = 0;
    size_t max                                    = 0;
    exseis::utils::Floating_point sample_interval = 0.0;
    bool lob                                      = false;
    bool random                                   = false;

    if (argc <= 1) {
        std::cout
          << "Options: filemake -o \"name.segy\" -s <ns> -t <nt> -m <mem(MiB)> -i <inc>\n";
        return EXIT_FAILURE;
    }

    // TODO: document these arguments:
    //           -o output file
    //           -s number of samples
    //           -t number of traces
    //           -m max
    //           -i increment
    //           -l lobsided
    //           -r 'random' offsets
    std::string opt = "s:t:m:o:i:lr";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'o':
                name = optarg;
                break;

            case 's':
                ns = std::stoul(optarg);
                break;

            case 't':
                nt = std::stoul(optarg);
                break;

            case 'm':
                max = std::stoul(optarg);
                break;

            case 'i':
                sample_interval = std::stod(optarg);
                break;

            case 'l':
                lob = true;
                break;

            case 'r':
                random = true;
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    assert(!name.empty() && max != 0 && sample_interval != 0.0);
    max *= 1024 * 1024;
    file_make(lob, random, name, max, ns, nt, sample_interval);

    return 0;
}
