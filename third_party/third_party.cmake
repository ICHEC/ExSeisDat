#
# Find MPI
#
include(${CMAKE_CURRENT_LIST_DIR}/find_mpi.cmake)


#
# Find FFTW3
#
include(${CMAKE_CURRENT_LIST_DIR}/find_fftw3.cmake)


#
# Find CLI11
#
add_library(exseisdat_CLI11 INTERFACE)
target_include_directories(
    exseisdat_CLI11
    SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/CLI11-v1.7.1
)
add_library(exseisdat::CLI11 ALIAS exseisdat_CLI11)
