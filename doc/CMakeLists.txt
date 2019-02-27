find_package(Doxygen REQUIRED)

# Write a Doxyfile including the main doc/Doxyfile.
# This sets the @INCLUDE_PATH variable to the project root.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

set(INCLUDE_DOXYFILE "@INCLUDE = Doxyfile")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-man.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-man
    @ONLY
)

# Get list of examples (files ending in .c or .cc)
file(
    GLOB EXSEISDAT_EXAMPLES
    RELATIVE ${CMAKE_SOURCE_DIR}/examples/
    ${CMAKE_SOURCE_DIR}/examples/*.c
    ${CMAKE_SOURCE_DIR}/examples/*.cc
)

# Loop over examples files, adding rules to turn them into their documented and
# clean versions.
# Keep a list of the documented versions so we can add them as dependencies to
# the documentation generation.
set(EXSEISDAT_DOCUMENTED_EXAMPLES)
foreach(example IN LISTS EXSEISDAT_EXAMPLES)

    # Add documented and clean example generation rule
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/examples/documented/${example}.md
            ${CMAKE_CURRENT_BINARY_DIR}/examples/clean/${example}

        COMMAND
            ${CMAKE_COMMAND}
                -DINPUT_FILENAME=${CMAKE_SOURCE_DIR}/examples/${example}
                -DOUTPUT_DOC_FILENAME=${CMAKE_CURRENT_BINARY_DIR}/examples/documented/${example}.md
                -DOUTPUT_CLEAN_FILENAME=${CMAKE_CURRENT_BINARY_DIR}/examples/clean/${example}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/parse_example.cmake

        MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/examples/${example}
    )

    # Add documented file to the list
    list(
        APPEND EXSEISDAT_DOCUMENTED_EXAMPLES
        ${CMAKE_CURRENT_BINARY_DIR}/examples/documented/${example}.md
    )
endforeach(example IN LISTS EXSEISDAT_EXAMPLES)

# Custom command to run doxygen on build/doc/Doxyfile in the build/doc dir.
# Running doxygen is tied to the html/index.html file.
# TODO: The main dependency is currently doc/Doxyfile, but there should be
#       a depencency on *every* file being documented!
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    DEPENDS ${EXSEISDAT_DOCUMENTED_EXAMPLES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building the documentation..."
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/man/exseis-extract-metadata.1
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-man"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-man.in
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-man.in
        ${EXSEISDAT_DOCUMENTED_EXAMPLES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building the documentation..."
)

# Target to wire up building the documentation to the ALL target.
add_custom_target(
    doc ALL
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
        ${CMAKE_CURRENT_BINARY_DIR}/man/exseis-extract-metadata.1
)

# Set install target
# Pro tip: To build and install just the documentation,
install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION ${EXSEISDAT_INSTALL_DOCDIR}
)
