
# Define the options

set(
    EXSEISDAT_TEST_DATA_DIR "tmp"
    CACHE PATH
    "The directory to read/write test data to/from."
)

option(
    EXSEISDAT_TEST_ENABLE_SPECTESTS
    "Run the single-core spectests"
    ON
)

option(
    EXSEISDAT_TEST_ENABLE_MPI_SPECTESTS
    "Run the multi-core spectests with MPI"
    ON
)

set(
    EXSEISDAT_TEST_NPROCS 4
    CACHE STRING
    "The number of processes to run the multi-core spec tests with"
)

option(
    EXSEISDAT_TEST_ENABLE_C_WRAP_TESTS
    "Run the C wrap tests"
    ON
)

option(
    EXSEISDAT_TEST_ENABLE_CMAKE_TESTS
    "Run the CMake project export tests."
    OFF
)


#
# Setup googletest dependencies
#
set(GOOGLETEST_DIR     ${CMAKE_CURRENT_BINARY_DIR}/googletest-release-1.8.0)
set(GOOGLETEST_ARCHIVE ${CMAKE_CURRENT_LIST_DIR}/googletest-release-1.8.0-patched.zip)

# Googletests requires pthreads on Linux
find_package(Threads REQUIRED)

# Unzip googletest
add_custom_command(
    OUTPUT
        ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
        ${GOOGLETEST_DIR}/googletest/include/gtest/gtest.h

        ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
        ${GOOGLETEST_DIR}/googlemock/include/gmock/gmock.h
    COMMAND
        ${CMAKE_COMMAND} -E tar xf ${GOOGLETEST_ARCHIVE}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    unpack_googletest
    DEPENDS
        ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
        ${GOOGLETEST_DIR}/googletest/include/gtest/gtest.h

        ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
        ${GOOGLETEST_DIR}/googlemock/include/gmock/gmock.h
)

include_directories(
    SYSTEM
    # Include googletest directories.
    ${GOOGLETEST_DIR}/googletest/include
    ${GOOGLETEST_DIR}/googletest
    ${GOOGLETEST_DIR}/googlemock/include
    ${GOOGLETEST_DIR}/googlemock
)
include_directories(
    # Include test directory
    ${CMAKE_CURRENT_LIST_DIR}

    # Include src directory
    ${CMAKE_CURRENT_LIST_DIR}/../src
)


#
# Setup the test executables
#

add_executable(genfiles genfiles.c)

add_executable(
    spectests

    spectests/tglobal.cc

    spectests/utils/logging/Verbosity.test.cc
    spectests/utils/signal_processing/Gain_function.test.cc
    spectests/utils/signal_processing/Taper_function.test.cc
    spectests/utils/typedefs.test.cc
    spectests/utils/Type.test.cc

    spectests/piol/Trace_metadata_key.test.cc
    spectests/piol/Trace_header_offsets.test.cc
    spectests/piol/operations/Sort_type.test.cc

    spectests/datampiioread.cc
    spectests/datampiiowrite.cc
    spectests/datatype.cc
    spectests/dynsegymd.cc
    spectests/filesegyintegread.cc
    spectests/filesegyintegwrite.cc
    spectests/filesegyread.cc
    spectests/filesegywrite.cc
    spectests/main.cc
    spectests/objsegyintegread.cc
    spectests/objsegyintegwrite.cc
    spectests/objsegymeta.cc
    spectests/objsegyread.cc
    spectests/objsegywrite.cc
    spectests/opstest.cc
    spectests/segy.cc
    spectests/segymd.cc
    spectests/segymdextra.cc
    spectests/selftests.cc
    spectests/set.cc
    spectests/utils/signal_processing/mute.test.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)
target_link_libraries(spectests PRIVATE exseisdat Threads::Threads)


add_library(
    wraptests

    wraptests/googletest_variable_instances.cc

    wraptests/intercepted_c_bindings.cc

    wraptests/checkreturnlistener.cc
    wraptests/exseis.cc
    wraptests/exseiswraptest.cc
    wraptests/getminmax.cc
    wraptests/getminmaxwraptests.cc
    wraptests/mockexseis.cc
    wraptests/mockgetminmax.cc
    wraptests/mockparam.cc
    wraptests/mockreadinterface.cc
    wraptests/mockrule.cc
    wraptests/mockset.cc
    wraptests/mockwriteinterface.cc
    wraptests/param.cc
    wraptests/paramwraptests.cc
    wraptests/printers.cc
    wraptests/readinterface.cc
    wraptests/readinterfacewraptests.cc
    wraptests/rule.cc
    wraptests/rulewraptests.cc
    wraptests/set.cc
    wraptests/setwraptests.cc
    wraptests/wraptests.cc
    wraptests/wraptesttools.cc
    wraptests/writeinterface.cc
    wraptests/writeinterfacewraptests.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)
target_link_libraries(wraptests PUBLIC exseisdat Threads::Threads)
set_target_properties(
    wraptests
    PROPERTIES DEFINE_SYMBOL "EXSEISDAT_WRAPTESTS_EXPORTS"
)

add_executable(c_wraptest cwraptests/main.c)
target_link_libraries(c_wraptest PRIVATE wraptests)

if(EXSEISDAT_ENABLE_SANITIZERS)
    set_target_properties(c_wraptest PROPERTIES LINKER_LANGUAGE CXX)
endif(EXSEISDAT_ENABLE_SANITIZERS)



#
# Create dependencies on test data created by genfile.
#

# Create the tmp directory
add_custom_target(make_tmp_directory)
add_custom_command(
    TARGET make_tmp_directory PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EXSEISDAT_TEST_DATA_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

function(add_genfile filename)
    set(_absolute_filename)
    get_filename_component(
        _absolute_filename "${EXSEISDAT_TEST_DATA_DIR}/${filename}"
        REALPATH
        BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}"
    )

    # Define how to create the test file using gentest
    add_custom_command(
        OUTPUT "${_absolute_filename}"
        COMMAND $<TARGET_FILE:genfiles> "${_absolute_filename}"
        DEPENDS make_tmp_directory genfiles
    )

    # Define a target depending on the file
    add_custom_target(
        ${filename} DEPENDS "${_absolute_filename}"
    )
    # Add the target as a dependency of spectests
    add_dependencies(spectests ${filename})
endfunction(add_genfile)

# Define all the test files
add_genfile(small_file_pattern.tmp)
add_genfile(large_file_pattern.tmp)
add_genfile(small_segy.tmp)
add_genfile(big_trace_segy.tmp)
add_genfile(large_segy.tmp)


#
# Add the test targets
#

# Run the spectests in serial
if(EXSEISDAT_TEST_ENABLE_SPECTESTS)
    # Run spectests with the *Death* and *Farm* tests disabled
    add_test(
        NAME spectests_test
        COMMAND
            spectests
                "${EXSEISDAT_TEST_DATA_DIR}"
                --gtest_filter=-*Death*:*Farm*
    )

    # Run just the *Death* tests
    add_test(
        NAME spectests_death_test
        COMMAND
            spectests
            "${EXSEISDAT_TEST_DATA_DIR}"
            --gtest_filter=*Death*
    )
endif(EXSEISDAT_TEST_ENABLE_SPECTESTS)


# Run the spectests in parallel
if(EXSEISDAT_TEST_ENABLE_MPI_SPECTESTS)
    # We might need the --oversubscribe flag on e.g. OpenMPI on macOS
    execute_process(
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4
                ${MPIEXEC_PREFLAGS} --oversubscribe
                    ${CMAKE_COMMAND} -E echo "testing --oversubscribe"
        RESULT_VARIABLE _EXSEISDAT_OVERSUBSCRIBE_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(_EXSEISDAT_OVERSUBSCRIBE_RESULT EQUAL 0)
        set(MPIEXEC_PREFLAGS "${MPIEXEC_PREFLAGS};--oversubscribe")
    endif(_EXSEISDAT_OVERSUBSCRIBE_RESULT EQUAL 0)


    add_test(
        NAME spectests_mpi_test
        COMMAND
            ${MPIEXEC}
                ${MPIEXEC_NUMPROC_FLAG} ${EXSEISDAT_TEST_NPROCS}
                ${MPIEXEC_PREFLAGS}
                $<TARGET_FILE:spectests>
                    ${MPIEXEC_POSTFLAGS}
                    "${EXSEISDAT_TEST_DATA_DIR}"
                    --gtest_filter=-*Death*:*Farm*
    )
endif(EXSEISDAT_TEST_ENABLE_MPI_SPECTESTS)

if(EXSEISDAT_TEST_ENABLE_C_WRAP_TESTS)
    add_test(
        NAME c_wraptest_test
        COMMAND c_wraptest
    )
endif(EXSEISDAT_TEST_ENABLE_C_WRAP_TESTS)

#
# Add test that ExSeisDat can be found by CMake
#
if(EXSEISDAT_TEST_ENABLE_CMAKE_TESTS)
    find_package(Git REQUIRED)
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../.git)
        message(FATAL_ERROR "The source directory must be a git clone for the cmake_subdirectory test!\nSet EXSEISDAT_TEST_ENABLE_CMAKE_SUBDIRECTORY_TEST=OFF to disable this message.")
    endif()

    # Test ExSeisDat can be included using add_subdirectory
    add_test(
        NAME cmake_add_subdirectory_test
        COMMAND
            ${CMAKE_COMMAND}
                -DTEST_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}/cmake/add_subdirectory
                -DEXSEISDAT_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}/..
                -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                -P ${CMAKE_CURRENT_LIST_DIR}/cmake/add_subdirectory/test.cmake
    )

    # Test ExSeisDat can be included using find_package after installation
    add_test(
        NAME cmake_find_package_test
        COMMAND
            ${CMAKE_COMMAND}
                -DTEST_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}/cmake/find_package
                -DEXSEISDAT_SOURCE_DIR=${CMAKE_CURRENT_LIST_DIR}/..
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                -P ${CMAKE_CURRENT_LIST_DIR}/cmake/find_package/test.cmake
    )
endif(EXSEISDAT_TEST_ENABLE_CMAKE_TESTS)
