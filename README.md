# ExSeisDat

ExSeisDat is a High Performance I/O library for seismic data files,
primarily the SEG-Y format.

It includes a low-level parallel file I/O library, ExSeisPIOL,
primarily targeting MPI I/O, containing routines for selectively loading and
processing meta-data and trace data.

Also included is a high-level library, ExSeisFlow,
which uses ExSeisPIOL to implement a number of seismic data workflows,
e.g. sorting data, binning, trace analysis, file concatenation, etc.


## Building Instructions

To configure and build ExSeisDat, a C++-14 compatible compiler is needed,
and CMake > 3.5.2.

    # Make a directory to hold the temporary build files
    mkdir -p /path/to/build/directory

    # Change directory to the build directory
    cd /path/to/build/directory

    # Configure the ExSeisDat build using CMake
    cmake \
        -DCMAKE_INSTALL_PREFIX=/path/to/install/directory \
        /path/to/the/exseisdat/project

    # Build the ExSeisDat project
    make

    # Test the ExSeisDat project
    make test

    # Install the ExSeisDat project
    make install

    # (Optional) clean up all the temporary build files
    # cd $HOME
    # rm -rf /path/to/build/directory

CMake should find the necessary MPI and FFTW libraries automatically.


### Specifying MPI

If CMake can't find MPI, or finds the wrong version of MPI, set
`export CC=/path/to/mpicc` and `export CXX=/path/to/mpicxx` in your environment
(optionally, also the appropriate MPI compiler variables, e.g. `I_MPI_CC` and
`I_MPI_CXX`, to find the correct `C` and `CXX` compilers) before running CMake
to find the correct MPI libraries.
If CMake doesn't find them after setting `CC` and `CXX`, you may need to delete
the build directory and reconfigure.

Alternatively, passing the `-DCMAKE_C_COMPILER=/path/to/mpicc` and
`-DCMAKE_CXX_COMPILER=/path/to/mpicxx` to the `cmake` command should do
all the necessary reconfiguration and recompilation necessary.


### Specifying FFTW

If CMake can't find FFTW, or finds the wrong version, try passing
`-DFFTW3_DIR=/path/to/fftw` to the `cmake` command.
The directory `/path/to/fftw` should be the directory including
`include/fftw3.h`, for a full path of `/path/to/fftw/include/fftw3.h`.
CMake will also look for `/path/to/fftw/local/include/fftw3.h`.
The FFTW library should be found at `/path/to/fftw/lib/libfftw3f.so`.
CMake fill also look in the `lib64` and `bin` directories, and the `local/lib`
directory (also `lib64` and `bin`) if fftw3.h was found in `local/include`.

If the Intel compiler is used, CMake will use the `-mkl` flag to link the Intel
MKL library instead of FFTW.


### Build Flags

CMake has a number of pre-configured build modes: mainly Release and Debug.

To build ExSeisDat in its release configuration
(`-O3`, no asserts, no debug symbols)
pass `-DCMAKE_BUILD_TYPE=Release` to the `cmake` command.
This is the default if this option isn't passed.

To build ExSeisDat in its debug configuration
(asserts enabled, debug symbols added)
pass `-DCMAKE_BUILD_TYPE=Debug` to the `cmake` command.

To add extra flags to the build, use the `-DCMAKE_C_FLAGS` and
`-DCMAKE_CXX_FLAGS` options.
For example, to include `-O0` and address sanitization for debug builds, pass

    cmake \
        -DCMAKE_CXX_FLAGS="-O0 -fsanitize=address" \
        /path/to/the/exseisdat/project


### A Suggestion

One easy way of developing ExSeisDat with CMake is to put the configuration /
build commands in a script file, have the script file in the project directory,
and have the build directory as a subdirectory in the project directory.

If we call it e.g. make.sh, we can write the following:

    #!/usr/bin/env bash
    # Contents of make.sh:

    # Exit script on first non-zero return
    set -o errexit

    # Get the directory containing this script!
    script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

    source_dir=${script_dir}
    build_dir=${script_dir}/build/build
    install_dir=${script_dir}/build/install

    mkdir -p ${build_dir}
    mkdir -p ${install_dir}

    # Load whatever modules are necessary
    # e.g. using the Intel compiler on Fionn:
    # module load dev
    # module load cmake/intel/3.5.2
    # module load intel/latest
    # module load openmpi/intel/1.8.3

    cd ${build_dir}
    cmake ${source_dir} \
        -DCMAKE_INSTALL_PREFIX=${install_dir} \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_C_COMPILER=icc \
        -DCMAKE_CXX_COMPILER=icpc \
        -DCMAKE_C_FLAGS="-O1 -fsanitize=address" \
        -DCMAKE_CXX_FLAGS="-O1 -fsanitize=address"
    cd ${script_dir}

    make -C ${build_dir}
    make -C ${build_dir} test
    make -C ${build_dir} install

Now, running `./make.sh` should configure, build, test, install all the
necessary files and components automatically.
When any files are changed in the project, running this command should
reconfigure and recompile only the minimum necessary files, and
run the test suite over the whole project.
Reconfiguration should be reasonably cheap, unless the CMakeLists.txt files
have been altered.

In this manner, flags and configuration options can be quickly changed, and
easily tracked.
Testing can also be disabled by simply commenting out the
`make -C ${build_dir} test` command.

Note: It can be easy to forget the trailing `\` for line continuation in
the `cmake` command!
If you get error messages like `Cannot find command -DCMAKE_...`, a missing
`\` is the likely culprit!


#### Modulefiles

Putting module commands here also makes the compilation environment clear.

Some care may be taken, as some environments aren't configured for
using `module` commands in a script by default.
Users may need to use `source /path/to/modules/init` to make `module`
available in the script.
On Fionn, for example, use `source /usr/share/modules/init/bash`.
