
#
# Setup all the utility executables
#

option(
    EXSEISDAT_INSTALL_APPS
    "Set whether or not the ExSeisDat utility programs should be installed."
    ON
)

#
# Install the utility executables
#
if(EXSEISDAT_INSTALL_APPS)
    set(
        EXSEISDAT_APPS_BINDIR ${EXSEISDAT_INSTALL_BINDIR} CACHE STRING
        "The directory to install the ExSeisDat utility programs to."
    )
endif(EXSEISDAT_INSTALL_APPS)

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_APPS_EXES.
set(EXSEISDAT_APPS_EXES)
macro(apps_add_executable)
    add_executable(${ARGV})
    target_link_libraries(${ARGV0} PUBLIC exseisdat)
    target_include_directories(${ARGV0} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if(EXSEISDAT_ENABLE_SANITIZERS)
        set_target_properties(${ARGV0} PROPERTIES LINKER_LANGUAGE CXX)
    endif(EXSEISDAT_ENABLE_SANITIZERS)

    if(EXSEISDAT_INSTALL_APPS)
        install(
            TARGETS ${ARGV0}
            RUNTIME DESTINATION ${EXSEISDAT_APPS_BINDIR}
        )
    endif(EXSEISDAT_INSTALL_APPS)
endmacro(apps_add_executable)


apps_add_executable(exseis-traceanalysis    exseis-traceanalysis.main.cc)

apps_add_executable(exseis-assess   exseis-assess.main.cc)

apps_add_executable(exseis-versort  exseis-verifysort.main.cc)

apps_add_executable(exseis-concatenate  exseis-concatenate.main.cc)
target_link_libraries(exseis-concatenate PUBLIC exseisdat::CLI11)

apps_add_executable(exseis-segsort  exseis-sort.main.cc)
target_link_libraries(exseis-segsort PUBLIC exseisdat::CLI11)

apps_add_executable(
    exseis-fourdbin
    exseis-fourdbin.main.cc fourdbin/4d.cc fourdbin/4dio.cc fourdbin/4dcore.cc
)

apps_add_executable(exseis-filemake exseis-filemake.main.cc)

apps_add_executable(exseis-makerep  exseis-makerep.main.cc)

apps_add_executable(exseis-makerepn1    exseis-makerep.main.cc)

apps_add_executable(exseis-makerepn2    exseis-makerep.main.cc)

apps_add_executable(exseis-minmax   exseis-minmax.main.cc)

apps_add_executable(exseis-segy2kml exseis-segy2kml.main.cc)

apps_add_executable(exseis-extract-metadata exseis-extract-metadata.main.cc)
target_link_libraries(exseis-extract-metadata PUBLIC exseisdat::CLI11)


