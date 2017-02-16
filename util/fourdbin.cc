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

int main(int argc, char ** argv)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;
    geom_t dsrmax = 0.5;            //Default dsdr criteria
    bool verbose = false;
/**********************************************************************************************************
 *******************  Reading options from the command line *********************************************** 
 **********************************************************************************************************/
    std::string opt = "a:b:c:d:t:v";  //TODO: uses a GNU extension
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
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());
/**********************************************************************************************************
 **********************************************************************************************************/

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    //Open the two input files
    File::Direct file1(piol, name1, FileMode::Read);
    File::Direct file2(piol, name2, FileMode::Read);

    std::unique_ptr<Coords> coords1;
    std::unique_ptr<Coords> coords2;
    size_t sz[2];
#ifndef HACK
    cmsg(piol, "Parameter-read phase");
    //Perform the decomposition and read the coordinates of interest.
    auto time = MPI_Wtime();
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;
        sz[1] = dec2.second;

        coords1 = std::make_unique<Coords>(sz[0]);
        assert(coords1.get());
        getCoords(ppiol, file1, dec1.first, coords1.get());

        cmsg(piol, "Read sets of coordinates from file " + name1 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
        time = MPI_Wtime();

        coords2 = std::make_unique<Coords>(sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2.get());

        cmsg(piol, "Read sets of coordinates from file " + name2 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
    }

    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);
    calc4DBin(piol, dsrmax, coords1.get(), coords2.get(), min, minrs, verbose);

    cmsg(piol, "Final list pass");

//Weed out traces that have a match that is too far away
    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
    vec<geom_t> lminrs(sz[0]);
    size_t cnt = 0U;

    vec<size_t> rjct1(sz[0]);
    vec<size_t> rjct2(sz[0]);
    vec<geom_t> rjctminrs(sz[0]);
    size_t rjct = 0;

    for (size_t i = 0U; i < sz[0]; i++)
        if (minrs[i] <= dsrmax)
        {
            list2[cnt] = min[i];
            lminrs[cnt] = minrs[i];
            list1[cnt++] = coords1->tn[i];
        }
        else
        {
            rjct2[rjct] = min[i];
            rjctminrs[rjct] = minrs[i];
            rjct1[rjct++] = coords1->tn[i];
        }

    list1.resize(cnt);
    list2.resize(cnt);
    lminrs.resize(cnt);

    rjct1.resize(rjct);
    rjct2.resize(rjct);
    rjctminrs.resize(rjct);

{
    std::string name = "tmp/vtn" + std::to_string(rank);
    FILE * fOut = fopen(name.c_str(), "w+");
    assert(1U == fwrite(&cnt, sizeof(size_t), 1U, fOut));
    assert(list1.size() == fwrite(list1.data(), sizeof(size_t), list1.size(), fOut));
    assert(list2.size() == fwrite(list2.data(), sizeof(size_t), list2.size(), fOut));
    assert(lminrs.size() == fwrite(lminrs.data(), sizeof(geom_t), lminrs.size(), fOut));
    fclose(fOut);

    name += ".txt";
    fOut = fopen(name.c_str(), "w+");
    fprintf(fOut, "N#\tlist1\tlist2\tminrs\n");
    for (size_t i = 0; i < cnt; i++)
        fprintf(fOut, "%zu\t%zu\t%zu\t%f\n", i, list1[i], list2[i], lminrs[i]);
    fclose(fOut);

    name += "_reject";
    fOut = fopen(name.c_str(), "w+");
    fprintf(fOut, "N#\trjct1\trjct2\trjctminrs\n");
    for (size_t i = 0; i < rjct; i++)
        fprintf(fOut, "%zu\t%zu\t%zu\t%f\n", i, rjct1[i], rjct2[i], rjctminrs[i]);
    fclose(fOut);
    return 0;
}
#endif
#ifdef HACK
    size_t cnt;
    std::string name = "tmp/vtn" + std::to_string(rank);
    FILE * fOut = fopen(name.c_str(), "r+");
    assert(1U == fread(&cnt, sizeof(size_t), 1U, fOut));
    vec<size_t> list1(cnt);
    vec<size_t> list2(cnt);
    vec<geom_t> lminrs(cnt);
    assert(list1.size() == fread(list1.data(), sizeof(size_t), list1.size(), fOut);
    assert(list2.size() == fread(list2.data(), sizeof(size_t), list2.size(), fOut);
    assert(lminrs.size() == fread(lminrs.data(), sizeof(geom_t), lminrs.size(), fOut);
    fclose(fOut);
#endif

    //free up some memory
    coords1.release();
    coords2.release();

    cmsg(piol, "Output phase");
    //Enable as many of the parameters as possible
    auto rule = std::make_shared<Rule>(true, true, true);
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);

    time = MPI_Wtime();
    //Open and write out file1 --> file3
    File::Direct file3(piol, name3, FileMode::Write);

    cmsg(piol, "Output 3");
    //select(piol, rule, file3, file1, list1, minrs);
    #warning the wrong but safer function
    selectDupe(piol, rule, file3, file1, list1, lminrs);

    cmsg(piol, "Output 3 in " + std::to_string(MPI_Wtime()- time) + " seconds");
    time = MPI_Wtime();
    //Open and write out file2 --> file4
    //This case is more complicated because the list is unordered and there can be duplicate entries
    //in the list.
    File::Direct file4(piol, name4, FileMode::Write);
    selectDupe(piol, rule, file4, file2, list2, lminrs);
    cmsg(piol, "Output 4 in " + std::to_string(MPI_Wtime()- time) + " seconds");

    return 0;
}
