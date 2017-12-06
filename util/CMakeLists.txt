# Link libexseisdat to all the executables.
link_libraries(exseisdat)

#
# Setup all the utility executables
#

set(EXSEISDAT_UTILITIES_EXES)

add_executable(exseis-cropen        cropen.c sglobal.c ctest.c)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-cropen)

add_executable(exseis-traceanalysis traceanalysis.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-traceanalysis)

add_executable(exseis-radon         radon.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-radon)

message(AUTHOR_WARNIG "No file radon_file.cc, expected in old util/makefile.")
#add_executable(exseis-radon_file    radon_file.cc sglobal.cc)
#list(APPEND EXSEISDAT_UTILITIES_EXES exseis-radon_file)

add_executable(exseis-assess        assess.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-assess)

add_executable(exseis-versort       verifysort.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-versort)

add_executable(exseis-concatenate   concatenate.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-concatenate)

add_executable(exseis-creadwrite    concatenate.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-creadwrite)

add_executable(exseis-segsort       sort.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-segsort)

add_executable(exseis-fourdbin      fourdbin.cc 4dio.cc 4dcore.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-fourdbin)

add_executable(exseis-filemake      filemake.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-filemake)

add_executable(exseis-makerep       makerep.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-makerep)

add_executable(exseis-makerepn1     makerep.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-makerepn1)

add_executable(exseis-makerepn2     makerep.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-makerepn2)

add_executable(exseis-minmax        minmax.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-minmax)

add_executable(exseis-segy2kml      segy2kml.cc sglobal.cc)
list(APPEND EXSEISDAT_UTILITIES_EXES exseis-segy2kml)


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
