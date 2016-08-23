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
namespace PIOL {
void printErr(ExSeisPIOL & piol, const std::string file, const Log::Layer layer, const int err, const MPI_Status * stat, std::string msg)
{
    if (err != MPI_SUCCESS)
    {
        if ((err == MPI_ERR_IN_STATUS) && (stat != NULL || stat != nullptr))
            msg += " MPI_Status: " + std::to_string(stat->MPI_ERROR);

        piol.record(file, layer, Log::Status::Error, msg, Log::Verb::None);

        std::vector<void *> buf(20);
        int num = backtrace(buf.data(), buf.size());
        backtrace_symbols_fd(buf.data(), num, 1);
    }
}
}
