# Link all the libraries with exseisdat
link_libraries(exseisdat)


#
# Setup googletest dependencies
#
set(GOOGLETEST_DIR     ${CMAKE_CURRENT_BINARY_DIR}/googletest-release-1.8.0)
set(GOOGLETEST_ARCHIVE ${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.8.0.zip)

# Unzip googletest
add_custom_command(
    OUTPUT
        ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
        ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
    COMMAND
        ${CMAKE_COMMAND} -E tar xf ${GOOGLETEST_ARCHIVE}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Include googletest directories.
include_directories(
    ${GOOGLETEST_DIR}/googletest/include
    ${GOOGLETEST_DIR}/googletest
    ${GOOGLETEST_DIR}/googlemock/include
    ${GOOGLETEST_DIR}/googlemock
)


#
# Setup the test executables
#

add_executable(extract extract.c)
add_executable(genfiles genfiles.c)

add_executable(
    spectests

    data.cc
    datampiioread.cc
    datampiiowrite.cc
    datatype.cc
    dynsegymd.cc
    file.cc
    filesegyintegread.cc
    filesegyintegwrite.cc
    filesegyread.cc
    filesegywrite.cc
    main.cc
    objsegyintegread.cc
    objsegyintegwrite.cc
    objsegymeta.cc
    objsegyread.cc
    objsegywrite.cc
    opstest.cc
    segy.cc
    segymd.cc
    segymdextra.cc
    selftests.cc
    set.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)


#
# Create dependencies on test data created by genfile.
#

# Create the tmp directory
add_custom_target(make_tmp_directory)
add_custom_command(
    TARGET make_tmp_directory PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tmp
)

function(add_genfile filename)
    # Define how to create the test file using gentest
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmp/${filename}
        COMMAND $<TARGET_FILE:genfiles> ${filename}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp
        DEPENDS make_tmp_directory
    )
    # Define a target depending on the file
    add_custom_target(
        ${filename} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tmp/${filename}
    )
    # Add the target as a dependency of spectests
    add_dependencies(spectests ${filename})
endfunction(add_genfile)

# Define all the test files
add_genfile(smallFilePattern.tmp)
add_genfile(largeFilePattern.tmp)
add_genfile(smallsegy.tmp)
add_genfile(bigtracesegy.tmp)
add_genfile(largesegy.tmp)


#
# Add the test targets
#

add_test(
    NAME spectests_test
    COMMAND spectests --gtest_filter=-*Farm*
)
set_tests_properties(spectests_test PROPERTIES DEPENDS genfiles_test)
