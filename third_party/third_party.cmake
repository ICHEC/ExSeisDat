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
add_library(exseisdat::CLI11 INTERFACE IMPORTED)
set_property(
    TARGET exseisdat::CLI11
    PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/CLI11-v1.7.1
)
set_property(
    TARGET exseisdat::CLI11
    PROPERTY
        INTERFACE_COMPILE_DEFINITIONS CLI11_EXPERIMENTAL_OPTIONAL=0
)


#
# Find Catch2
#
add_library(exseisdat::catch2 INTERFACE IMPORTED)
set_property(
    TARGET exseisdat::catch2
    PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/catch-v2.12.1
)
