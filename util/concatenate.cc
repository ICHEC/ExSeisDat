#include <glob.h>
#include <assert.h>
#include "cppfileapi.hh"
#include "data/datampiio.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "set/set.hh"
#include "sglobal.hh"
#include <iostream>
#include <regex>
#include <unordered_map>
using namespace PIOL;

typedef std::unordered_map<size_t, std::vector<std::pair<std::string, size_t>>> FileMap;
FileMap getFileMap(std::shared_ptr<ExSeisPIOL> piol, std::string pattern)
{
    size_t rank = piol->comm->getRank();
    glob_t globs;
    int err = glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK, NULL, &globs);
    if (err)
        exit(-1);

    std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);

    if (rank)
        std::cout << "File Count: " << globs.gl_pathc << "\n";

    FileMap map;
    for (size_t i = 0; i < globs.gl_pathc; i++)
        if (std::regex_match(globs.gl_pathv[i], reg))
        {
            if (!rank && !(i % 100))
                std::cout << i << " of " << globs.gl_pathc << std::endl;

            std::string name = globs.gl_pathv[i];
            auto data = std::make_shared<Data::MPIIO>(piol, name, FileMode::Read);
            auto obj = std::make_shared<Obj::SEGY>(piol, name, data, FileMode::Read);
            auto file = std::make_shared<File::SEGY>(piol, name, obj, FileMode::Read);

            piol->isErr();
            map[file->readNs()].emplace_back(std::pair<std::string, size_t>(name, file->readNt()));
        }
    globfree(&globs);
    return map;
}

void printStats(FileMap & map)
{
    std::cout << "Processed all files\n";
    for (auto & l : map)
    {
        std::cout << "ns " << l.first;
        size_t nt = 0;
        for (size_t i = 0; i < l.second.size(); i++)
        {
            size_t inNt = l.second[i].second;
            nt += inNt;
        }
        std::cout << " Total number of traces " << nt << std::endl;
    }
}

void doPrompt(ExSeisPIOL * piol)
{
    char cont = '0';
    if (!piol->comm->getRank())
    {
        std::cout << "Continue concatenation? (y/n)\n";
        std::cin >> cont;
    }
    int err = MPI_Bcast(&cont, 1U, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS)
    {
        std::cerr << "Scatter error\n";
        exit(-1);
    }

    if (cont != 'y')
    {
        std::cout << "Exit\n";
        exit(0);
    }
    else if (!piol->comm->getRank())
        std::cout << "Continuing\n";
}

std::shared_ptr<File::Rule> getMaxRules(void)
{
    auto rule = std::make_shared<File::Rule>(true, true, true);
    rule->addLong(File::Meta::Misc1, File::Tr::TransConst);
    rule->addShort(File::Meta::Misc2, File::Tr::TransExp);
    //Override the default behaviour of ShotNum
    rule->addLong(File::Meta::ShotNum, File::Tr::ShotNum);
    rule->addShort(File::Meta::Misc3, File::Tr::ShotScal);
    return rule;
}

size_t readwriteAll(ExSeisPIOL * piol, size_t doff, std::shared_ptr<File::Rule> rule, File::Interface * dst, File::Interface * src)
{
    const size_t memlim = 2U*1024U*1024U*1024U;
    auto dec = decompose(src->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    size_t offset = dec.first;
    size_t lnt = dec.second;
    size_t ns = src->readNs();
    size_t max = memlim / (2U*SEGSz::getDOSz(ns) + rule->memusage());
    size_t fmax = std::min(max, lnt);

    auto biggest = piol->comm->max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    File::Param prm(rule, fmax);
    std::vector<trace_t> trc(ns * fmax);
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        src->readTrace(offset + i, rblock, trc.data(), &prm);
        dst->writeTrace(doff + offset + i, rblock, trc.data(), &prm);
    }
    for (size_t i = 0; i < extra; i++)
    {
        src->readTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
        dst->writeTrace(size_t(0), size_t(0), nullptr, (File::Param *)nullptr);
    }
    return src->readNt();
}

int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few arguments\n";
        return -1;
    }
    ExSeis piol;

    std::string pattern = "";
    std::string outprefix = "";
    std::string msg = "Concatenated with ExSeisPIOL";
    bool prompt = true;
    bool setlayer = false;
    std::string opt = "i:o:m:sl";  //TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                pattern = optarg;
                if (!piol.getRank())
                    std::cout << "Pattern: " << pattern << "\n";
            break;
            case 'o' :
                outprefix = optarg;
                if (!piol.getRank())
                    std::cout << "output prefix: " << outprefix << "\n";
            break;
            case 'm' :
                msg = optarg;
            break;
            case 's' :
                prompt = false;
            break;
            case 'l' :
            setlayer = true;
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;

        }

    if (setlayer)
    {
        std::cout << "set layer\n";
        Set(piol, pattern, outprefix);
        return 0;
    }

    auto map = getFileMap(piol, pattern);

    if (!piol.getRank())
        printStats(map);

    if (prompt)
        doPrompt(piol);

    auto rule = getMaxRules();

    for (auto & l : map)
    {
        size_t ns = l.first;
        size_t nt = 0;
        geom_t inc = 0.0;

        File::Direct out(piol, outprefix + std::to_string(ns) + ".sgy", FileMode::Write);
        out.writeNs(ns);
        if (!piol.getRank())
            std::cout << "Processing ns " << ns << "\n";

        for (size_t i = 0; i < l.second.size(); i++)
        {
            std::string iname = l.second[i].first;
            size_t inNt = l.second[i].second;
            //TODO: Single process read, write optimisation for small files?
            File::Direct in(piol, iname, FileMode::Read);
            assert(in.readNs() == ns);  //Sanity check.
            assert(in.readNt() == inNt);  //Sanity check.

            nt += readwriteAll(piol, nt, rule, out, in);
            inc = in.readInc();
        }
        out.writeNt(nt);
        out.writeInc(inc);
        out.writeText(msg + "\n");
    }
    piol.isErr();
    return 0;
}

