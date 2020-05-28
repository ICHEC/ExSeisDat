#
# Setup all the utility executables
#

add_library(sglobal OBJECT sglobal.cc)
target_include_directories(
    sglobal PUBLIC $<TARGET_PROPERTY:exseisdat,INCLUDE_DIRECTORIES>
)

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_UTILITIES_EXES.
set(EXSEISDAT_UTILITIES_EXES)
macro(util_add_executable)
    add_executable(${ARGV} $<TARGET_OBJECTS:sglobal>)
    list(APPEND EXSEISDAT_UTILITIES_EXES ${ARGV0})
    target_link_libraries(${ARGV0} exseisdat)

    if(EXSEISDAT_ENABLE_SANITIZERS)
        set_target_properties(${ARGV0} PROPERTIES LINKER_LANGUAGE CXX)
    endif(EXSEISDAT_ENABLE_SANITIZERS)
endmacro(util_add_executable)


util_add_executable(exseis-cropen   cropen.c)

util_add_executable(exseis-traceanalysis    traceanalysis.cc)

util_add_executable(exseis-radon    radon.cc)

util_add_executable(exseis-assess   assess.cc)

util_add_executable(exseis-versort  verifysort.cc)

util_add_executable(exseis-concatenate  concatenate.cc)

util_add_executable(exseis-creadwrite   concatenate.cc)

util_add_executable(exseis-segsort  sort.cc)

util_add_executable(exseis-fourdbin fourdbin.cc 4d.cc 4dio.cc 4dcore.cc)

util_add_executable(exseis-filemake filemake.cc)

util_add_executable(exseis-makerep  makerep.cc)

util_add_executable(exseis-makerepn1    makerep.cc)

util_add_executable(exseis-makerepn2    makerep.cc)

util_add_executable(exseis-minmax   minmax.cc)

util_add_executable(exseis-segy2kml segy2kml.cc)

util_add_executable(exseis-extract-metadata extract-metadata.main.cc)
target_link_libraries(exseis-extract-metadata exseisdat::CLI11)


#
# Install the utility executables
#
option(
    EXSEISDAT_INSTALL_UTILITIES
    "Set whether or not the ExSeisDat utility programs should be installed."
    ON
)
if(EXSEISDAT_INSTALL_UTILITIES)
    set(
        EXSEISDAT_UTILITIES_BINDIR ${EXSEISDAT_INSTALL_BINDIR} CACHE STRING
        "The directory to install the ExSeisDat utility programs to."
    )

    install(
        TARGETS ${EXSEISDAT_UTILITIES_EXES}
        RUNTIME DESTINATION ${EXSEISDAT_UTILITIES_BINDIR}
    )

endif(EXSEISDAT_INSTALL_UTILITIES)
