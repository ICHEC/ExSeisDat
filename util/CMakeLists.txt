# Link libexseisdat to all the executables.
link_libraries(exseisdat)

#
# Setup all the utility executables
#

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_UTILITIES_EXES.
set(EXSEISDAT_UTILITIES_EXES)
macro(util_add_executable)
    add_executable(${ARGV})
    list(APPEND EXSEISDAT_UTILITIES_EXES ${ARGV0})
endmacro(util_add_executable)


util_add_executable(exseis-cropen   cropen.c)

util_add_executable(exseis-traceanalysis    traceanalysis.cc)

util_add_executable(exseis-radon    radon.cc sglobal.cc)

util_add_executable(exseis-assess   assess.cc sglobal.cc)

util_add_executable(exseis-versort  verifysort.cc sglobal.cc)

util_add_executable(exseis-concatenate  concatenate.cc sglobal.cc)

util_add_executable(exseis-creadwrite   concatenate.cc sglobal.cc)

util_add_executable(exseis-segsort  sort.cc)

util_add_executable(exseis-fourdbin fourdbin.cc 4dio.cc 4dcore.cc sglobal.cc)

util_add_executable(exseis-filemake filemake.cc sglobal.cc)

util_add_executable(exseis-makerep  makerep.cc sglobal.cc)

util_add_executable(exseis-makerepn1    makerep.cc sglobal.cc)

util_add_executable(exseis-makerepn2    makerep.cc sglobal.cc)

util_add_executable(exseis-minmax   minmax.cc sglobal.cc)

util_add_executable(exseis-segy2kml segy2kml.cc sglobal.cc)


#
# Install the utility executables
#
set(
    EXSEISDAT_INSTALL_UTILITIES ON CACHE BOOL
    "Set whether or not the ExSeisDat utility programs should be installed."
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
