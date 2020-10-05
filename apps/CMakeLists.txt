
#
# Setup all the utility executables
#

add_library(sglobal OBJECT sglobal.cc)
target_include_directories(
    sglobal PUBLIC $<TARGET_PROPERTY:exseisdat,INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_options(
    sglobal PUBLIC $<TARGET_PROPERTY:exseisdat,INTERFACE_COMPILE_OPTIONS>
)
target_compile_features(
    sglobal PUBLIC $<TARGET_PROPERTY:exseisdat,INTERFACE_COMPILE_FEATURES>
)

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_APPS_EXES.
set(EXSEISDAT_APPS_EXES)
macro(apps_add_executable)
    add_executable(${ARGV} $<TARGET_OBJECTS:sglobal>)
    list(APPEND EXSEISDAT_APPS_EXES ${ARGV0})
    target_link_libraries(${ARGV0} exseisdat)

    if(EXSEISDAT_ENABLE_SANITIZERS)
        set_target_properties(${ARGV0} PROPERTIES LINKER_LANGUAGE CXX)
    endif(EXSEISDAT_ENABLE_SANITIZERS)
endmacro(apps_add_executable)


apps_add_executable(exseis-traceanalysis    traceanalysis.cc)

apps_add_executable(exseis-assess   assess.cc)

apps_add_executable(exseis-versort  verifysort.cc)

apps_add_executable(exseis-concatenate  concatenate.cc)
target_link_libraries(exseis-concatenate exseisdat::CLI11)

apps_add_executable(exseis-segsort  sort.cc)
target_link_libraries(exseis-segsort exseisdat::CLI11)

apps_add_executable(exseis-fourdbin fourdbin.cc 4d.cc 4dio.cc 4dcore.cc)

apps_add_executable(exseis-filemake filemake.cc)

apps_add_executable(exseis-makerep  makerep.cc)

apps_add_executable(exseis-makerepn1    makerep.cc)

apps_add_executable(exseis-makerepn2    makerep.cc)

apps_add_executable(exseis-minmax   minmax.cc)

apps_add_executable(exseis-segy2kml segy2kml.cc)

apps_add_executable(exseis-extract-metadata extract-metadata.main.cc)
target_link_libraries(exseis-extract-metadata exseisdat::CLI11)


#
# Install the utility executables
#
option(
    EXSEISDAT_INSTALL_APPS
    "Set whether or not the ExSeisDat utility programs should be installed."
    ON
)
if(EXSEISDAT_INSTALL_APPS)
    set(
        EXSEISDAT_APPS_BINDIR ${EXSEISDAT_INSTALL_BINDIR} CACHE STRING
        "The directory to install the ExSeisDat utility programs to."
    )

    install(
        TARGETS ${EXSEISDAT_APPS_EXES}
        RUNTIME DESTINATION ${EXSEISDAT_APPS_BINDIR}
    )
endif(EXSEISDAT_INSTALL_APPS)
