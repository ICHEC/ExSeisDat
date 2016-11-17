#include <glob.h>
#include <assert.h>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include <iostream>
#include <regex>
#include <unordered_map>
using namespace PIOL;
int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few arguments\n";
        return -1;
    }
    ExSeis piol;

    glob_t globs;
    if (!piol.getRank())
        std::cout << "Pattern: " << argv[1] << "\n";
    int err = glob(argv[1], GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err)
        return -1;

    std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);

    if (!piol.getRank())
        std::cout << "File Count: " << globs.gl_pathc << "\n";

    std::unordered_map<size_t, std::vector<std::pair<std::string, size_t>>> list;

    for (size_t i = 0; i < globs.gl_pathc; i++)
        if (std::regex_match(globs.gl_pathv[i], reg))
        {
            std::string name = globs.gl_pathv[i];
    //        std::cout << "File: " << name << "\n";
            File::Direct file(piol, name, FileMode::Read);
            if (!piol.getRank() && !(i % 100))
                std::cout << i << " of " << i << std::endl;

            piol.isErr();
            list[file.readNs()].emplace_back(std::pair<std::string, size_t>(name, file.readNt()));
        }

    char cont = '0';
    if (!piol.getRank())
    {
        std::cout << "Processed all files\n";
        for (auto & l : list)
        {
            std::cout << "ns " << l.first;
        //std::cout << "ns " << l.first << std::endl;
            size_t nt = 0;
            for (size_t i = 0; i < l.second.size(); i++)
            {
     //       std::string iname = l.second[i].first;
                size_t inNt = l.second[i].second;
                nt += inNt;
//            std::cout << iname << " " << inNt << std::endl;
            }
            std::cout << " Total number of traces " << nt << std::endl;
        }

        std::cout << "Continue concatenation? (y/n)\n";
        std::cin >> cont;
    }
    err = MPI_Bcast(&cont, 1U, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS)
    {
        std::cerr << "Scatter error\n";
        return -1;
    }

    if (cont != 'y')
    {
        std::cout << "Exit\n";
        return 0;
    }
    else if (!piol.getRank())
        std::cout << "Continuing\n";

    //TODO: add continue check
    const size_t memlim = 2U*1024U*1024U*1024U;

    auto rule = std::make_shared<File::Rule>(true, true, true);

    rule->addLong(File::Meta::Misc1, File::Tr::TransConst);
    rule->addShort(File::Meta::Misc2, File::Tr::TransExp);

    //auto rule = std::make_shared<File::Rule>(true, true, true);
    for (auto & l : list)
    {
        size_t ns = l.first;
        size_t nt = 0;
        geom_t inc = 0.0;

        File::Direct out(piol, "out_ns" + std::to_string(ns) + ".sgy", FileMode::Write);
        out.writeNs(ns);
        if (!piol.getRank())
            std::cout << "output " << ns << std::endl;

        for (size_t i = 0; i < l.second.size(); i++)
        {
            std::string iname = l.second[i].first;
            size_t inNt = l.second[i].second;
/*            if (SEGSz::getFileSz(inNt, ns) < 8U*1024U*1024U)
            {
//TODO: Single process read, collective write
            }
            else
*/
            if (!piol.getRank())
                std::cout << "input " << iname << " ";

            {
                File::Direct in(piol, iname, FileMode::Read);
                assert(in.readNs() == ns);  //Sanity check.
                assert(in.readNt() == inNt);  //Sanity check.
                auto dec = decompose(inNt, piol.getNumRank(), piol.getRank());
                size_t offset = dec.first;
                size_t lnt = dec.second;

                size_t max = memlim / (2U*SEGSz::getDOSz(ns) + rule->memusage());
                size_t fmax = std::min(max, lnt);

                auto biggest = ((ExSeisPIOL *)piol)->comm->max(lnt);
                size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

                if (!piol.getRank())
                    std::cout << fmax << " " << extra << " " << biggest << std::endl;
                File::Param prm(rule, fmax);
                std::vector<trace_t> trc(ns * fmax);
                for (size_t i = 0; i < lnt; i += max)
                {
                    size_t rblock = (i + max < lnt ? max : lnt - i);
                    in.readTrace(offset + i, rblock, trc.data(), &prm);
                    out.writeTrace(nt + offset + i, rblock, trc.data(), &prm);
                }
                for (size_t i = 0; i < extra; i++)
                {
                    in.readTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
                    out.writeTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
                }
                inc = in.readInc();
            }

            nt += inNt;
        }
        out.writeNt(nt);
        out.writeInc(inc);
        out.writeText("Concatenated with ExSeisPIOL");
    }
    piol.isErr();
    globfree(&globs);
    return 0;
}
