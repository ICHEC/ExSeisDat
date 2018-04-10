#
# Add the example executables
#

# Link exseisdat to all executables defined here
link_libraries(exseisdat)

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_EXAMPLE_EXES.
set(EXSEISDAT_EXAMPLE_EXES)
macro(examples_add_executable)
    add_executable(${ARGV})
    list(APPEND EXSEISDAT_EXAMPLE_EXES ${ARGV0})
endmacro(examples_add_executable)

examples_add_executable(hello_world hello_world.c)

examples_add_executable(creadwrite creadwrite.c)

examples_add_executable(example1  example1.cc)
examples_add_executable(cexample1 example1.c)

examples_add_executable(cexample2 example2.c)

examples_add_executable(minmax  minmax.cc)
examples_add_executable(cminmax minmax.c)

examples_add_executable(modprm modprm.c)

examples_add_executable(sort  sort.cc)
examples_add_executable(csort sort.c)

examples_add_executable(exampleCreateFile  exampleCreateFile.cc)
examples_add_executable(cexampleCreateFile exampleCreateFile.c)

examples_add_executable(exampleFilterTaper exampleFilterTaper.cc)

examples_add_executable(cexampleSort exampleSort.c)
examples_add_executable(exampleSort exampleSort.cc)


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
