#
# Test ExSeisDat can be added as a subdirectory and used with find_package
# from a CMakeLists.txt script.
#
# Note: this should be run from a build directory
#

# Expected variables:
#   TEST_SOURCE_DIR: The source directory for the cmake_subdirectory test
#   EXSEISDAT_SOURCE_DIR: The project root for the exseisdat source
#   GIT_EXECUTABLE: A git executable, for cloning the exseisdat source dir
#   BUILD_SHARED_LIBS: This is passed along to the CMake command.

function(assert_execute_process)
    set(_ret)
    execute_process(${ARGN} RESULT_VARIABLE _ret)
    if(NOT _ret EQUAL 0)
        message(FATAL_ERROR "execute_process failed: ${ARGN}")
    endif()
endfunction()


file(REMOVE_RECURSE cmake_find_package_test)
file(
    COPY ${TEST_SOURCE_DIR}/
    DESTINATION cmake_find_package_test
)

# clone / build / install ExSeisDat
set(
    EXSEISDAT_BASE_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_find_package_test/exseisdat
)
set(EXSEISDAT_BUILD_DIR "${EXSEISDAT_BASE_DIR}/build")
set(EXSEISDAT_INSTALL_DIR "${EXSEISDAT_BASE_DIR}/install")

file(MAKE_DIRECTORY ${EXSEISDAT_BUILD_DIR})
file(MAKE_DIRECTORY ${EXSEISDAT_INSTALL_DIR})

assert_execute_process(
    COMMAND
        ${CMAKE_COMMAND} ${EXSEISDAT_SOURCE_DIR}
            -DCMAKE_VERBOSE_MAKEFILE=ON
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -DCMAKE_INSTALL_PREFIX=${EXSEISDAT_INSTALL_DIR}
            -DEXSEISDAT_BUILD_TESTS=NO
            -DEXSEISDAT_BUILD_DOCUMENTATION=NO
            -DEXSEISDAT_BUILD_APPS=NO
            -DEXSEISDAT_BUILD_EXAMPLES=NO
    WORKING_DIRECTORY ${EXSEISDAT_BUILD_DIR}
)
assert_execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${EXSEISDAT_BUILD_DIR}
)
assert_execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${EXSEISDAT_BUILD_DIR} --target install
)

#
# configure / build the dummy project
#

# From the build directory
file(MAKE_DIRECTORY cmake_find_package_test/build_from_build)
assert_execute_process(
    COMMAND
        ${CMAKE_COMMAND} ..
            -DCMAKE_VERBOSE_MAKEFILE=ON
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -Dexseisdat_DIR=${EXSEISDAT_BUILD_DIR}
    WORKING_DIRECTORY cmake_find_package_test/build_from_build
)
assert_execute_process(
    COMMAND ${CMAKE_COMMAND} --build build_from_build
    WORKING_DIRECTORY cmake_find_package_test
)

# From the install directory
file(MAKE_DIRECTORY cmake_find_package_test/build_from_install)
assert_execute_process(
    COMMAND
        ${CMAKE_COMMAND} ..
            -DCMAKE_VERBOSE_MAKEFILE=ON
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -DCMAKE_INSTALL_PREFIX=${EXSEISDAT_INSTALL_DIR}
    WORKING_DIRECTORY cmake_find_package_test/build_from_install
)
assert_execute_process(
    COMMAND ${CMAKE_COMMAND} --build build_from_install
    WORKING_DIRECTORY cmake_find_package_test
)
