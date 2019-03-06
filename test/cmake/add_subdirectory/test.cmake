#
# Test ExSeisDat can be added as a subdirectory and used with add_subdirectory
# from a CMakeLists.txt script.
#
# Note: this should be run from a build directory
#

# Expected variables:
#   TEST_SOURCE_DIR: The source directory for the cmake_subdirectory test
#   EXSEISDAT_SOURCE_DIR: The project root for the exseisdat source
#   GIT_EXECUTABLE: A git executable, for cloning the exseisdat source dir
#   BUILD_SHARED_LIBS: This is passed along to the CMake command.

file(REMOVE_RECURSE cmake_add_subdirectory_test)
file(
    COPY ${TEST_SOURCE_DIR}/
    DESTINATION cmake_add_subdirectory_test
)
execute_process(
    COMMAND
        ${GIT_EXECUTABLE}
            clone --depth=1 file://${EXSEISDAT_SOURCE_DIR} exseisdat
    WORKING_DIRECTORY cmake_add_subdirectory_test
)
file(MAKE_DIRECTORY cmake_add_subdirectory_test/build)

execute_process(
    COMMAND
        ${CMAKE_COMMAND} ..
            -DCMAKE_VERBOSE_MAKEFILE=ON
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    WORKING_DIRECTORY cmake_add_subdirectory_test/build
)
execute_process(
    COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY cmake_add_subdirectory_test/build
)
