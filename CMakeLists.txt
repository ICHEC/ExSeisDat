cmake_minimum_required(VERSION 3.5.2)

project(
    ExSeisDat
    VERSION 0.0.1
    LANGUAGES C CXX
)


# Outline CMake options ########################################################

# We need c++14 at minimum.
# This should maybe be changed to feature options for flexibility with less
# standards compliant compilers.
set(
    CMAKE_CXX_STANDARD 14 CACHE STRING
    "The C++ standard to target for compilation."
)

# CMAKE_CXX_STANDARD not explicitly supported for Intel until CMake 3.6
if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")


#
# Setup subfolder options.
#
# Provide options to build or skip building the programs/libraries in the
# given subdirectories.
#
option(
    EXSEISDAT_BUILD_UTILITIES
    "Build ExSeisDat utility programs."
    ON
)

option(
    EXSEISDAT_BUILD_EXAMPLES
    "Build ExSeisDat example programs."
    ON
)

option(
    EXSEISDAT_BUILD_TESTS
    "Build ExSeisDat tests. Depends on googletest."
    ON
)

option(
    EXSEISDAT_BUILD_DOCUMENTATION
    "Build ExSeisDat documentation"
    ON
)


#
# Setup installation path options.
#
# Initially set paths to GNU defaults.
# Users can specify directories using the various EXSEISDAT_INSTALL_XXX
# variables.
# e.g. setting the documentation install dir
#   cmake -DEXSEISDAT_INSTALL_DOCDIR=/path/to/docs /path/to/exseisdat
#

# Get some sane default install directories
include(GNUInstallDirs)

set(
    EXSEISDAT_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE STRING
    "The install directory for ExSeisDat executables."
)

set(
    EXSEISDAT_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING
    "The install directory for ExSeisDat libraries."
)

set(
    EXSEISDAT_INSTALL_DOCDIR ${CMAKE_INSTALL_DOCDIR} CACHE STRING
    "The install directory for ExSeisDat libraries."
)


#
# Setup build type defaults.
#
# We want to use the release build by default.
# That is, if a user says nothing about the build type, build the release
# version.
#
if(NOT CMAKE_BUILD_TYPE)
    get_property(
        _CMAKE_BUILD_TYPE_HELPSTRING
        CACHE CMAKE_BUILD_TYPE
        PROPERTY HELPSTRING
    )
    set(
        CMAKE_BUILD_TYPE "Release" CACHE STRING
        "${_CMAKE_BUILD_TYPE_HELPSTRING}"
        FORCE
    )
    unset(_CMAKE_BUILD_TYPE_HELPSTRING)
endif(NOT CMAKE_BUILD_TYPE)


# Find needed libraries ########################################################

include(FindPackageHandleStandardArgs)

#
# Find MPI
#
find_package(MPI REQUIRED)

# Set MPI C/CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_C_COMPILE_FLAGS}")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${MPI_C_COMPILE_FLAGS}")

# Include the MPI directories
include_directories(SYSTEM ${MPI_C_INCLUDE_PATH})

# Set the MPI linker flags
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${MPI_C_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_C_LINK_FLAGS} ${MPI_CXX_LINK_FLAGS}")


#
# Find fftw3
#
# Look for fftw3 under the path provided by FFTW3_DIR on the command line,
# or in the system directories.
#

# Find the fftw3 include directory.
# Find the include directory by looking for fftw3.h
find_path(
    FFTW3_INCLUDES fftw3.h
    HINTS ${FFTW3_DIR}
    PATH_SUFFIXES include local/include
)

# Check FFTW3_INCLUDES contains a path, and output that value once.
# Otherwise, exit with an error message.
find_package_handle_standard_args(FFTW3_INCLUDES DEFAULT_MSG FFTW3_INCLUDES)
if(NOT FFTW3_INCLUDES)
    message(FATAL_ERROR "Unable to find FFTW3 headers!")
endif(NOT FFTW3_INCLUDES)


# Find the fftw3 single precision library.
# Using the Intel compiler, try the -mkl flag, otherwise look for libfftw3f.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(FFTW3_LIBRARIES -mkl)
else()
    find_library(
        FFTW3_LIBRARIES NAMES fftw3f
        HINTS ${FFTW3_DIR} ${FFTW3_INCLUDES}/..
        PATH_SUFFIXES bin lib lib64
    )
endif()

# Check FFTW3_LIBRARIES contains a library, and output that value once.
# Otherwise, exit with an error message.
find_package_handle_standard_args(FFTW3_LIBRARIES DEFAULT_MSG FFTW3_LIBRARIES)
if(NOT FFTW3_LIBRARIES)
    message(FATAL_ERROR "Unable to find FFTW3 libraries!")
endif(NOT FFTW3_LIBRARIES)


# Setup header includes ########################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(SYSTEM ${FFTW3_INCLUDES})


# Setup the exseisdat library ##################################################

add_library(
    exseisdat

    src/PIOL_C_bindings.cc

    src/Flow/Cache.cc
    src/Flow/Flow_C_bindings.cc
    src/Flow/RadonGatherState.cc
    src/Flow/Set.cc

    src/CommunicatorMPI.cc
    src/DataMPIIO.cc
    src/ExSeis.cc
    src/ExSeisPIOL.cc
    src/Logger.cc
    src/ObjectInterface.cc
    src/ObjectSEGY.cc
    src/Param.cc
    src/ReadDirect.cc
    src/ReadInterface.cc
    src/ReadModel.cc
    src/ReadSEGY.cc
    src/ReadSEGYModel.cc
    src/Rule.cc
    src/WriteDirect.cc
    src/WriteInterface.cc
    src/WriteSEGY.cc

    src/param_utils.cc
    src/segy_utils.cc

    src/gather.cc
    src/minmax.cc
    src/sort.cc
    src/temporalfilter.cc

    src/utils/decomposition/block_decomposition.cc
    src/utils/encoding/character_encoding.cc
    src/utils/encoding/number_encoding.cc
    src/utils/mpi/MPI_error_to_string.cc
    src/utils/signal_processing/AGC.cc
    src/utils/signal_processing/Gain_function.cc
    src/utils/signal_processing/taper.cc
    src/utils/signal_processing/Taper_function.cc
)

target_link_libraries(
    exseisdat
    PUBLIC ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} ${FFTW3_LIBRARIES}
)
set_target_properties(
    exseisdat
    PROPERTIES COMPILE_DEFINITIONS EXSEISDAT_MPIIO_COLLECTIVES
)
target_include_directories(
    exseisdat BEFORE
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)


# Include subdirectories #######################################################

# Output the directories that are being built.
message(STATUS "Subprojects:")
message(STATUS "    EXSEISDAT_BUILD_UTILITIES     = ${EXSEISDAT_BUILD_UTILITIES}")
message(STATUS "    EXSEISDAT_BUILD_EXAMPLES      = ${EXSEISDAT_BUILD_EXAMPLES}")
message(STATUS "    EXSEISDAT_BUILD_TESTS         = ${EXSEISDAT_BUILD_TESTS}")
message(STATUS "    EXSEISDAT_BUILD_DOCUMENTATION = ${EXSEISDAT_BUILD_DOCUMENTATION}")

# Build utilities
if(EXSEISDAT_BUILD_UTILITIES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/util)
endif(EXSEISDAT_BUILD_UTILITIES)

# Build examples
if(EXSEISDAT_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif(EXSEISDAT_BUILD_EXAMPLES)

# Build tests
if(EXSEISDAT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(EXSEISDAT_BUILD_TESTS)

# Build documentation
if(EXSEISDAT_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif(EXSEISDAT_BUILD_DOCUMENTATION)


# Setup installation directories ###############################################

# Install libexseisdat
install(
    TARGETS exseisdat
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# TODO: setup install for headers
