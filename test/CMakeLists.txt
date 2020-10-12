
# Define the options

option(
    EXSEISDAT_TEST_ENABLE_UNIT_TESTS
    "Run the single-core unit tests"
    ON
)

option(
    EXSEISDAT_TEST_ENABLE_MPI_UNIT_TESTS
    "Run the multi-core unit tests with MPI"
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


include_directories(
    # Include test directory
    ${CMAKE_CURRENT_LIST_DIR}

    # Include src directory
    ${CMAKE_CURRENT_LIST_DIR}/../src
)


#
# Unit tests
#

add_executable(
    unit.test

    unit.test.main.cc

    unit/exseis/test/Buffer.test.cc
    unit/exseis/test/Index_hash.test.cc
    unit/exseis/test/segy/Segy_file_generator.test.cc

    unit/exseis/utils/distributed_vector/Distributed_vector_mpi.test.cc
    unit/exseis/utils/encoding/character_encoding.test.cc
    unit/exseis/utils/encoding/number_encoding.test.cc
    unit/exseis/utils/signal_processing/mute.test.cc
    unit/exseis/utils/types/Type.test.cc

    unit/exseis/piol/io_driver/IO_driver_distributed_vector.test.cc
    unit/exseis/piol/io_driver/IO_driver_mpi.test.cc
    unit/exseis/piol/file/Input_file_segy.test.cc
    unit/exseis/piol/file/Output_file_segy.test.cc
    unit/exseis/piol/metadata/rules/Rule.test.cc
    unit/exseis/piol/metadata/rules/Segy_rule_entry.test.cc
    unit/exseis/piol/operations/minmax.test.cc
    unit/exseis/piol/operations/sort.test.cc
    unit/exseis/piol/segy/utils.test.cc
)

target_link_libraries(unit.test exseisdat exseisdat::catch2)
target_include_directories(unit.test PRIVATE unit src)


#
# Add the test targets
#

# Run the spectests in serial
if(EXSEISDAT_TEST_ENABLE_UNIT_TESTS)
    add_test(
        NAME unit.test
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 1
                $<TARGET_FILE:unit.test>
                ${MPIEXEC_POSTFLAGS}
    )
endif(EXSEISDAT_TEST_ENABLE_UNIT_TESTS)


# Run the spectests in parallel
if(EXSEISDAT_TEST_ENABLE_MPI_UNIT_TESTS)
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
        NAME unit.test_mpi
        COMMAND
            ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${EXSEISDAT_TEST_NPROCS}
                ${MPIEXEC_PREFLAGS}
                $<TARGET_FILE:unit.test>
                ${MPIEXEC_POSTFLAGS}
    )
endif(EXSEISDAT_TEST_ENABLE_MPI_UNIT_TESTS)


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
