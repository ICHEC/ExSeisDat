///
/// Example: Hello, World!    {#examples_hello_world_c}
/// =====================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// In this example, we'll look at initializing a PIOL object, potentially
/// across multiple processes, and do some operation on just one of those
/// processes.
///
///
/// Implementation
/// --------------
///
/// First, our boilerplate.
/// We'll include the ExSeisDat header, and any extras we need.
/// We also define a `main()` function.
///

#include "ExSeisDat/PIOL.h"

#include <stdio.h>

int main()
{

    /// We initialize the ExSeis object, frequently referred to in this project
    /// as the PIOL object.

    // Initialize the PIOL
    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);

    /// Here, we've initialized the PIOL with the lowest verbosity setting.
    /// By default, this will initialize using MPI over MPI_COMM_WORLD.
    /// That means any operations run using this PIOL object will run on all
    /// the launched processes.
    ///
    /// Next, we'll run some code on just one process. In this case, we'll get
    /// process 0 to print the number of processes available.

    // Run on rank 0.
    if (!PIOL_ExSeis_getRank(piol)) {
        // Get num ranks for the PIOL and print to stdout.
        size_t num = PIOL_ExSeis_getNumRank(piol);
        if (num == 1) {
            printf("Hello, World! There is 1 process\n");
        }
        else {
            printf("Hello, World! There are %zu processes\n", num);
        }
    }

    /// Having process 0 do serial operations is a common occurrence in MPI,
    /// and indeed many parallel codes. Process 0 is, then, often referred to as
    /// the master process.
    ///
    /// Having done the work we set out to do, we will now delete the PIOL
    /// object. This step is important to ensure files and caches are shut down
    /// properly when the program closes.

    // Cleanup the PIOL.
    PIOL_ExSeis_delete(piol);

    /// And finally, return 0 to indicate success.

    return 0;
}
///
/// Complete Code
/// -------------
///
/// @include hello_world.c
