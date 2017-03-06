/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date November 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#warning temp
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "4dio.hh"
#include "4dcore.hh"

using namespace PIOL;
using namespace FOURD;
using File::Tr;
using File::Rule;

namespace PIOL {
    void cmsg(ExSeisPIOL * piol, std::string msg)
    {
        piol->comm->barrier();
        if (!piol->comm->getRank())
            std::cout << msg << std::endl;
    }
}

void printAllLists(bool verbose, size_t rank, vec<size_t> & list1, vec<size_t> & list2, vec<fourd_t> & lminrs)
{
    if (verbose)
    {
        size_t cnt = list1.size();
        assert(list1.size() == list2.size() && list1.size() == lminrs.size());
        std::string name = "tmp/vtn" + std::to_string(rank);
        FILE * fOut = fopen(name.c_str(), "w+");
        assert(1U == fwrite(&cnt, sizeof(size_t), 1U, fOut));
        assert(list1.size() == fwrite(list1.data(), sizeof(size_t), list1.size(), fOut));
        assert(list2.size() == fwrite(list2.data(), sizeof(size_t), list2.size(), fOut));
        assert(lminrs.size() == fwrite(lminrs.data(), sizeof(fourd_t), lminrs.size(), fOut));
        fclose(fOut);

        name += ".txt";
        fOut = fopen(name.c_str(), "w+");
        fprintf(fOut, "N#\tlist1\tlist2\tminrs\n");
        for (size_t i = 0; i < cnt; i++)
            fprintf(fOut, "%zu\t%zu\t%zu\t%f\n", i, list1[i], list2[i], lminrs[i]);
        fclose(fOut);
    }
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    fourd_t dsrmax = 1.0;            //Default dsdr criteria
    bool verbose = false;
    bool skipToOutput = false;
/**********************************************************************************************************
 *******************  Reading options from the command line ***********************************************
 **********************************************************************************************************/
    std::string opt = "a:b:c:d:t:v:s";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    std::string name3 = "";
    std::string name4 = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'a' :
                name1 = optarg;
            break;
            case 'b' :
                name2 = optarg;
            break;
            case 'c' :
                name3 = optarg;
            break;
            case 'd' :
                name4 = optarg;
            break;
            case 't' :
                dsrmax = std::stod(optarg);
            break;
            case 'v' :
                verbose = true;
                cmsg(piol, "Verbose mode enabled");
            break;
            case 's' :
                skipToOutput = true;
                cmsg(piol, "Skipping to output stage");
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());
/**********************************************************************************************************
 **********************************************************************************************************/

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    vec<size_t> list1;
    vec<size_t> list2;
    vec<fourd_t> lminrs;

    //Open the two input files
    File::Direct file1(piol, name1, FileMode::Read);
    File::Direct file2(piol, name2, FileMode::Read);

    if (!skipToOutput)
    {
        cmsg(piol, "Parameter-read phase");
        //Perform the decomposition and read the coordinates of interest.
        auto time = MPI_Wtime();
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);
        auto coords1 = getCoords(piol, file1, dec1);

        cmsg(piol, "Read sets of coordinates from file " + name1 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
        time = MPI_Wtime();

        auto coords2 = getCoords(piol, file2, dec2);

        cmsg(piol, "Read sets of coordinates from file " + name2 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");

        vec<size_t> min(coords1->sz);
        vec<fourd_t> minrs(coords1->sz);
        calc4DBin(piol, dsrmax, coords1.get(), coords2.get(), min, minrs, verbose);

        cmsg(piol, "Final list pass");

//Weed out traces that have a match that is too far away

        list1.resize(coords1->sz);
        list2.resize(coords1->sz);
        lminrs.resize(coords1->sz);
        size_t cnt = 0U;

        for (size_t i = 0U; i < coords1->sz; i++)
            if (minrs[i] <= dsrmax)
            {
                list2[cnt] = min[i];
                lminrs[cnt] = minrs[i];
                list1[cnt++] = coords1->tn[i];
            }
        //free up some memory
        coords1.release();
        coords2.release();

        list1.resize(cnt);
        list2.resize(cnt);
        lminrs.resize(cnt);

        printAllLists(verbose, rank, list1, list2, lminrs);
    }
    else
    {
        size_t cnt;
        std::string name = "tmp/vtn" + std::to_string(rank);
        FILE * fOut = fopen(name.c_str(), "r+");
        assert(1U == fread(&cnt, sizeof(size_t), 1U, fOut));

        list1.resize(cnt);
        list2.resize(cnt);
        lminrs.resize(cnt);

        assert(list1.size() == fread(list1.data(), sizeof(size_t), list1.size(), fOut));
        assert(list2.size() == fread(list2.data(), sizeof(size_t), list2.size(), fOut));
        assert(lminrs.size() == fread(lminrs.data(), sizeof(fourd_t), lminrs.size(), fOut));
        fclose(fOut);
    }

    cmsg(piol, "Output phase");
    //Enable as many of the parameters as possible
    auto rule = std::make_shared<Rule>(true, true, true);


    //Note: Set to TimeScal for OpenCPS viewing of dataset.
    //OpenCPS is restrictive on what locations can be used
    //as scalars.
    rule->addSEGYFloat(Meta::dsdr, Tr::SrcMeas, Tr::TimeScal);

    auto time = MPI_Wtime();
    {
        //Open and write out file1 --> file3
        File::Direct file3(piol, name3, FileMode::Write);
        cmsg(piol, "Output 3");
        //select(piol, rule, file3, file1, list1, minrs);
        outputNonMono(piol, rule, file3, file1, list1, lminrs);
    }

    cmsg(piol, "Output 3 in " + std::to_string(MPI_Wtime()- time) + " seconds");
    time = MPI_Wtime();

    {
        //Open and write out file2 --> file4
        //This case is more complicated because the list is unordered and there can be duplicate entries
        //in the list.
        File::Direct file4(piol, name4, FileMode::Write);
        outputNonMono(piol, rule, file4, file2, list2, lminrs);
    }

    cmsg(piol, "Output 4 in " + std::to_string(MPI_Wtime()- time) + " seconds");
    return 0;
}
