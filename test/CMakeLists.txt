
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
    spectests-legacy.test

    spectests-legacy/main.cc

    spectests-legacy/tglobal.cc

    spectests-legacy/utils/Type.test.cc

    spectests-legacy/datatype.cc
    spectests-legacy/dynsegymd.cc
    spectests-legacy/filesegyintegread.cc
    spectests-legacy/filesegyintegwrite.cc
    spectests-legacy/filesegyread.cc
    spectests-legacy/filesegywrite.cc
    spectests-legacy/objsegyintegread.cc
    spectests-legacy/objsegyintegwrite.cc
    spectests-legacy/objsegymeta.cc
    spectests-legacy/objsegyread.cc
    spectests-legacy/objsegywrite.cc
    spectests-legacy/opstest.cc
    spectests-legacy/segy.cc
    spectests-legacy/segymd.cc
    spectests-legacy/segymdextra.cc
    spectests-legacy/selftests.cc
    spectests-legacy/set.cc
    spectests-legacy/utils/signal_processing/mute.test.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)
target_link_libraries(spectests-legacy.test PRIVATE exseisdat Threads::Threads)


#
# Unit tests
#

add_executable(
    unit.test

    unit.test.main.cc

    unit/exseisdat/test/Buffer.test.cc
    unit/exseisdat/test/Index_hash.test.cc
    unit/exseisdat/test/segy/Segy_file_generator.test.cc

    unit/exseisdat/utils/distributed_vector/Distributed_vector_mpi.test.cc
    unit/exseisdat/utils/encoding/character_encoding.test.cc
    unit/exseisdat/utils/encoding/number_encoding.test.cc
    unit/exseisdat/utils/signal_processing/mute.test.cc

    unit/exseisdat/piol/io_driver/IO_driver_distributed_vector.test.cc
    unit/exseisdat/piol/io_driver/IO_driver_mpi.test.cc
    unit/exseisdat/piol/file/Input_file_segy.test.cc
    unit/exseisdat/piol/file/Output_file_segy.test.cc
)

target_link_libraries(unit.test exseisdat exseisdat::catch2)
target_include_directories(unit.test PRIVATE unit src)



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
    add_dependencies(spectests-legacy.test ${filename})
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
        NAME spectests-legacy.test
        COMMAND
            spectests-legacy.test
                "${EXSEISDAT_TEST_DATA_DIR}"
                --gtest_filter=-*Death*:*Farm*
    )

    # Run just the *Death* tests
    add_test(
        NAME spectests-legacy.test_death
        COMMAND
            spectests-legacy.test
            "${EXSEISDAT_TEST_DATA_DIR}"
            --gtest_filter=*Death*
    )

    add_test(
        NAME unit.test
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 1
                $<TARGET_FILE:unit.test>
                ${MPIEXEC_POSTFLAGS}
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
        NAME spectests-legacy.test_mpi
        COMMAND
            ${MPIEXEC}
                ${MPIEXEC_NUMPROC_FLAG} ${EXSEISDAT_TEST_NPROCS}
                ${MPIEXEC_PREFLAGS}
                $<TARGET_FILE:spectests-legacy.test>
                    ${MPIEXEC_POSTFLAGS}
                    "${EXSEISDAT_TEST_DATA_DIR}"
                    --gtest_filter=-*Death*:*Farm*
    )


    add_test(
        NAME unit.test_mpi
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${EXSEISDAT_TEST_NPROCS}
                ${MPIEXEC_PREFLAGS}
                $<TARGET_FILE:unit.test>
                ${MPIEXEC_POSTFLAGS}
    )
endif(EXSEISDAT_TEST_ENABLE_MPI_SPECTESTS)


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
