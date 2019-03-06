#
# Find fftw3
#
# Look for fftw3 under the path provided by FFTW3_DIR on the command line,
# or in the system directories.
#

include(FindPackageHandleStandardArgs)

if(EXSEISDAT_USE_MKL_FFTW3)
    find_path(
        FFTW3f_INCLUDE_DIRS fftw3.h
        HINTS ${FFTW3_DIR} ENV MKLROOT
        PATH_SUFFIXES include/fftw local/include/fftw
    )

    set(FFTW3f_LIBRARIES_FULL -mkl)
else(EXSEISDAT_USE_MKL_FFTW3)
    option(
        EXSEISDAT_USE_SYSTEM_FIND_FFTW3
        "Use the built-in FFTW3fConfig.cmake"
        OFF
    )
    
    if(EXSEISDAT_USE_SYSTEM_FIND_FFTW3)
        find_package(
            FFTW3f REQUIRED
            HINTS ${FFTW3f_DIR} ${FFTW3_DIR}
            PATH_SUFFIXES fftw3 cmake/fftw3 lib/cmake/fftw3
        )
    endif()

    find_path(
        FFTW3f_INCLUDE_DIRS fftw3.h
        HINTS ${FFTW3f_DIR} ${FFTW3_DIR}
        PATH_SUFFIXES include local/include
    )

    find_library(
        FFTW3f_LIBRARIES_FULL NAMES ${FFTW3f_LIBRARIES} fftw3f
        HINTS
            ${FFTW3f_LIBRARY_DIRS}
            ${FFTW3f_DIR} ${FFTW3_DIR}
            ${FFTW3f_INCLUDE_DIRS}/..
        PATH_SUFFIXES bin lib lib64
    )
endif(EXSEISDAT_USE_MKL_FFTW3)

# Check FFTW3f_INCLUDE_DIRS contains a path, and output that value once.
# Otherwise, exit with an error message.
find_package_handle_standard_args(
    FFTW3f
    REQUIRED_VARS
        FFTW3f_INCLUDE_DIRS
        FFTW3f_LIBRARIES_FULL
)


if(FFTW3f_FOUND AND NOT TARGET FFTW3::fftw3f)
    add_library(FFTW3::fftw3f INTERFACE IMPORTED)
    set_property(
        TARGET FFTW3::fftw3f
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${FFTW3f_INCLUDE_DIRS}
    )
    set_property(
        TARGET FFTW3::fftw3f
        PROPERTY INTERFACE_LINK_LIBRARIES ${FFTW3f_LIBRARIES_FULL}
    )
endif()
