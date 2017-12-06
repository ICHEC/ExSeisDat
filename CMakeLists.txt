cmake_minimum_required(VERSION 3.10)

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


#
# Setup subfolder options
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
    OFF
)


#
# Setup installation path options
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


# Setup header includes ########################################################

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)


# Setup the exseisdat library ##################################################

add_library(
    exseisdat

    # Public API sources
    api/cfileapi.cc
    api/cppfileapi.cc

    # Private API sources
    src/agc.cc
    src/cache.cc
    src/cmpi.cc
    src/datampiio.cc
    src/decomp.cc
    src/dynsegymd.cc
    src/flow.cc
    src/gather.cc
    src/iconv.cc
    src/log.cc
    src/minmax.cc
    src/misc.cc
    src/object.cc
    src/objsegy.cc
    src/piol.cc
    src/rfile.cc
    src/rfilesegy.cc
    src/sdatatype.cc
    src/segymd.cc
    src/smpi.cc
    src/sort.cc
    src/taper.cc
    src/temporalfilter.cc
    src/wfilesegy.cc

    # utils
    #TODO Remove me!
    util/sglobal.cc
    util/sglobal.c
)


# Include subdirectories #######################################################

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


# Setup installation directories ###############################################

# Install libexseisdat
install(
    TARGETS exseisdat
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# TODO: setup install for headers
