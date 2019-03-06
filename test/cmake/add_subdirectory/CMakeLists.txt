cmake_minimum_required(VERSION 3.10)

project(
    exseisdat_cmake_add_subdirectory_test
    VERSION 0.0.1
    LANGUAGES CXX
)


add_subdirectory(exseisdat EXCLUDE_FROM_ALL)

add_executable(test_exe dummy.cc)
target_link_libraries(test_exe PUBLIC exseisdat::exseisdat)
