// NOLINTNEXTLINE
#define CATCH_CONFIG_DEFAULT_REPORTER "mpi"
#define CATCH_CONFIG_RUNNER
#include "exseisdat/test/catch2.hh"

#include "exseisdat/test/Catch_reporter_mpi.hh"
#include "exseisdat/test/Test_case_cache.hh"

#include <mpi.h>

using exseis::test::Catch_reporter_mpi;
CATCH_REGISTER_REPORTER("mpi", Catch_reporter_mpi)

using exseis::test::Test_case_cache_listener;
CATCH_REGISTER_LISTENER(Test_case_cache_listener)

int main(int argc, char* argv[])
{

    MPI_Init(&argc, &argv);

    int result = Catch::Session().run(argc, argv);

    MPI_Finalize();

    return result;
}
