/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <execinfo.h>
#include "share/smpi.hh"
#warning temp
#include <iostream>
namespace PIOL {
void printErr(ExSeisPIOL & piol, const std::string file, const Log::Layer layer, const int err, const MPI_Status * stat, std::string msg)
{
    if (err != MPI_SUCCESS)
    {
        if ((err == MPI_ERR_IN_STATUS) && (stat != NULL || stat != nullptr))
            msg += " MPI_Status: " + std::to_string(stat->MPI_ERROR);
        else
            msg += "MPI_ERR = " + std::to_string(err);
        piol.record(file, layer, Log::Status::Error, msg, Log::Verb::None);

        if ((err == MPI_ERR_IN_STATUS) && (stat != NULL || stat != nullptr))
            std::cerr << " MPI_Status: " << stat->MPI_ERROR << std::endl;
//        else
//            std::cerr << "MPI_ERR = " << err << std::endl << std::endl;
/*        std::vector<void *> buf(20);
        int num = backtrace(buf.data(), buf.size());
        backtrace_symbols_fd(buf.data(), num, 1);*/
    }
}
}
