cmake_minimum_required(VERSION 3.10)

project(
    exseisdat
    VERSION 0.0.1
    LANGUAGES C CXX
)


# Outline CMake options ########################################################

# Determine if the compiler is Intel
if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(EXSEISDAT_COMPILER_IS_INTEL ON)
else()
    set(EXSEISDAT_COMPILER_IS_INTEL FALSE)
endif()


#
# Setup subfolder options.
#
# Provide options to build or skip building the programs/libraries in the
# given subdirectories.
#
option(
    EXSEISDAT_BUILD_APPS
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

option(
    EXSEISDAT_SET_DEFAULT_BUILD_TYPE
    "Set the default CMAKE_BUILD_TYPE to Release"
    OFF
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
    EXSEISDAT_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING
    "The install directory for ExSeisDat headers."
)

set(
    EXSEISDAT_INSTALL_DOCDIR ${CMAKE_INSTALL_DOCDIR} CACHE STRING
    "The install directory for ExSeisDat documentation."
)

set(
    EXSEISDAT_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR} CACHE STRING
    "The install directory for data (e.g. cmake scripts)."
)


#
# Setup build type defaults.
#
# We want to use the release build by default.
# That is, if a user says nothing about the build type, build the release
# version.
#
if(EXSEISDAT_SET_DEFAULT_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE)
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
endif(EXSEISDAT_SET_DEFAULT_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE)


#
# Setup compiler flags for development
#
option(
    EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS
    "Enable the flags used during ExSeisDat continuous integration builds."
    OFF
)


# Find needed libraries ########################################################

include(cmake/find_mpi.cmake)
include(third_party/third_party.cmake)


# Setup the exseisdat library ##################################################

add_library(
    exseisdat

    src/exseisdat/piol/configuration/ExSeis.cc
    src/exseisdat/piol/configuration/ExSeisPIOL.cc
    src/exseisdat/piol/file/Input_file_segy.cc
    src/exseisdat/piol/file/Output_file_segy.cc
    src/exseisdat/piol/io_driver/IO_driver_mpi.cc
    src/exseisdat/piol/metadata/rules/Rule.cc
    src/exseisdat/piol/metadata/Trace_metadata.cc
    src/exseisdat/piol/operations/gather.cc
    src/exseisdat/piol/operations/minmax.cc
    src/exseisdat/piol/operations/sort.cc
    src/exseisdat/piol/segy/utils.cc

    src/exseisdat/utils/communicator/Communicator_mpi.cc
    src/exseisdat/utils/decomposition/block_decomposition.cc
    src/exseisdat/utils/encoding/character_encoding.cc
    src/exseisdat/utils/encoding/number_encoding.cc
    src/exseisdat/utils/logging/Log.cc
    src/exseisdat/utils/logging/Status.cc
    src/exseisdat/utils/mpi_utils/MPI_error_to_string.cc
    src/exseisdat/utils/mpi_utils/MPI_max_array_length.cc
    src/exseisdat/utils/signal_processing/AGC.cc
    src/exseisdat/utils/signal_processing/Gain_function.cc
    src/exseisdat/utils/signal_processing/Taper_function.cc
    src/exseisdat/utils/signal_processing/mute.cc
    src/exseisdat/utils/signal_processing/taper.cc
)

target_compile_features(exseisdat PUBLIC cxx_std_14)

# Set DEFINE_SYMBOL to all-caps value to pass the clang-tidy check requiring
# all macro definitions to be upper-case.
set_target_properties(
    exseisdat
    PROPERTIES DEFINE_SYMBOL "EXSEISDAT_EXPORTS"
)

target_include_directories(
    exseisdat BEFORE
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)

target_include_directories(
    exseisdat PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(exseisdat PUBLIC MPI::MPI_CXX)

if(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)
    include(cmake/development_flags.cmake)
    target_link_libraries(exseisdat PUBLIC development_flags)
endif(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)

add_library(exseisdat::exseisdat ALIAS exseisdat)


# Include subdirectories #######################################################

# Output the directories that are being built.
message(STATUS "Subprojects:")
message(STATUS "    EXSEISDAT_BUILD_APPS          = ${EXSEISDAT_BUILD_APPS}")
message(STATUS "    EXSEISDAT_BUILD_EXAMPLES      = ${EXSEISDAT_BUILD_EXAMPLES}")
message(STATUS "    EXSEISDAT_BUILD_TESTS         = ${EXSEISDAT_BUILD_TESTS}")
message(STATUS "    EXSEISDAT_BUILD_DOCUMENTATION = ${EXSEISDAT_BUILD_DOCUMENTATION}")

# Build utilities
if(EXSEISDAT_BUILD_APPS)
    add_subdirectory(apps)
endif(EXSEISDAT_BUILD_APPS)

# Build examples
if(EXSEISDAT_BUILD_EXAMPLES)
    add_subdirectory(examples)
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

# Optionally define the development_flags target in the exports
set(_development_flags)
if(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)
    message("Enabling")
    set(_development_flags development_flags)
endif(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)

# Install libexseisdat
install(
    TARGETS exseisdat ${_development_flags}
    EXPORT exseisdat-targets
    RUNTIME DESTINATION ${EXSEISDAT_INSTALL_BINDIR}
    LIBRARY DESTINATION ${EXSEISDAT_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${EXSEISDAT_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${EXSEISDAT_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${EXSEISDAT_INSTALL_INCLUDEDIR}
)

# Install config files and targets
install(
    EXPORT exseisdat-targets
    NAMESPACE exseisdat::
    DESTINATION ${EXSEISDAT_INSTALL_DATADIR}/exseisdat/cmake
)

export(
    EXPORT exseisdat-targets
    NAMESPACE exseisdat::
)

# Configure exseisdat-config.cmake
include(CMakePackageConfigHelpers)
function(configure_find_file FIND_FILE RESULT)
    file(READ ${FIND_FILE} FIND_FILE_0)
    string(
        REPLACE "find_package(" "find_dependency("
        FIND_FILE_1 "${FIND_FILE_0}"
    )
    string(REPLACE "REQUIRED" "" FIND_FILE_2 "${FIND_FILE_1}")
    set(${RESULT} "${FIND_FILE_2}" PARENT_SCOPE)
endfunction(configure_find_file)
configure_find_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/find_mpi.cmake FIND_MPI
)
configure_package_config_file(
    cmake/exseisdat-config.cmake.in exseisdat-config.cmake
    INSTALL_DESTINATION ${EXSEISDAT_INSTALL_DATADIR}/exseisdat/cmake
)

write_basic_package_version_file(
    exseisdat-config-version.cmake
    COMPATIBILITY ExactVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/exseisdat-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/exseisdat-config-version.cmake
    DESTINATION ${EXSEISDAT_INSTALL_DATADIR}/exseisdat/cmake
)
