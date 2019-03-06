#
# Add the example executables
#

# Macro calls add_executable with the passed arguments, and adds the executable
# target to EXSEISDAT_EXAMPLE_EXES.
set(EXSEISDAT_EXAMPLE_EXES)
macro(examples_add_executable)
    add_executable(${ARGV})
    list(APPEND EXSEISDAT_EXAMPLE_EXES ${ARGV0})
    target_link_libraries(${ARGV0} PRIVATE exseisdat)

    if(EXSEISDAT_ENABLE_SANITIZERS)
        set_target_properties(${ARGV0} PROPERTIES LINKER_LANGUAGE CXX)
    endif(EXSEISDAT_ENABLE_SANITIZERS)
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


#
# Install the example executables
#
option(
    EXSEISDAT_INSTALL_EXAMPLES
    "Set whether or not the ExSeisDat example programs should be installed."
    OFF
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
