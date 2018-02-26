#
# Add the example executables
#

# Link exseisdat to all executables defined here
link_libraries(exseisdat)

# Keep a list of executables defined here
set(EXSEISDAT_EXAMPLE_EXES)

add_executable(hello_world hello_world.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES hello_world)

add_executable(creadwrite creadwrite.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES creadwrite)

add_executable(example1  example1.cc)
add_executable(cexample1 example1.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES example1 cexample1)

add_executable(cexample2 example2.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES cexample2)

add_executable(minmax  minmax.cc)
add_executable(cminmax minmax.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES minmax cminmax)

add_executable(modprm modprm.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES modprm)

add_executable(sort  sort.cc)
add_executable(csort sort.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES sort csort)

add_executable(exampleCreateFile  exampleCreateFile.cc)
add_executable(cexampleCreateFile exampleCreateFile.c)
list(APPEND EXSEISDAT_EXAMPLE_EXES exampleCreateFile cexampleCreateFile)

add_executable(exampleFilterTaper exampleFilterTaper.cc)
list(APPEND EXSEISDAT_EXAMPLE_EXES exampleFilterTaper)

add_executable(cexampleSort exampleSort.c)
add_executable(exampleSort exampleSort.cc)
list(APPEND EXSEISDAT_EXAMPLE_EXES exampleSort cexampleSort)

#
# Install the example executables
#
set(
    EXSEISDAT_INSTALL_EXAMPLES OFF CACHE BOOL
    "Set whether or not the ExSeisDat example programs should be installed."
)
if(EXSEISDAT_INSTALL_EXAMPLES)
    set(
        EXSEISDAT_EXAMPLES_BINDIR
        ${CMAKE_INSTALL_LIBEXECDIR}/exseisdat/examples CACHE STRING
        "The directory to install the ExSeisDat example programs to."
    )

    install(
        TARGETS ${EXSEISDAT_EXAMPLE_EXES}
        RUNTIME DESTINATION ${EXSEISDAT_EXAMPLES_BINDIR}
    )

endif(EXSEISDAT_INSTALL_EXAMPLES)
