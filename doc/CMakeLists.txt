find_package(Doxygen REQUIRED)

# Write a Doxyfile including the main doc/Doxyfile.
# This sets the @INCLUDE_PATH variable to the project root.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

# Custom command to run doxygen on build/doc/Doxyfile in the build/doc dir.
# Running doxygen is tied to the html/index.html file.
# TODO: The main dependency is currently doc/Doxyfile, but there should be
#       a depencency on *every* file being documented!
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building the documentation..."
)

# Target to wire up building the documentation to the ALL target.
add_custom_target(
    doc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
)

# Set install target
# Pro tip: To build and install just the documentation,
install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION ${EXSEISDAT_INSTALL_DOCDIR}
)
