cmake_minimum_required(VERSION 3.10)

project(
    exseisdat_cmake_find_package_test
    VERSION 0.0.1
    LANGUAGES CXX
)

find_package(exseisdat REQUIRED)

add_executable(test_exe dummy.cc)
target_link_libraries(test_exe PUBLIC exseisdat::exseisdat)
