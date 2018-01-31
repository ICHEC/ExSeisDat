#include "ctest.h"

void CTest_quit(const char* msg, size_t line)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (!rank) fprintf(stderr, msg, line);
    MPI_Finalize();
    exit(-1);
}
