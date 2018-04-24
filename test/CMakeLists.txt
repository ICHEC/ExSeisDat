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

include_directories(
    SYSTEM
    # Include googletest directories.
    ${GOOGLETEST_DIR}/googletest/include
    ${GOOGLETEST_DIR}/googletest
    ${GOOGLETEST_DIR}/googlemock/include
    ${GOOGLETEST_DIR}/googlemock
)
include_directories(
    # Include test directory
    ${CMAKE_CURRENT_SOURCE_DIR}

    # Include src directory
    ${CMAKE_SOURCE_DIR}/src
)


#
# Setup the test executables
#

add_executable(extract extract.c)
target_link_libraries(extract exseisdat)

add_executable(genfiles genfiles.c)
target_link_libraries(genfiles exseisdat)

add_executable(
    spectests

    spectests/tglobal.cc

    spectests/data.cc
    spectests/datampiioread.cc
    spectests/datampiiowrite.cc
    spectests/datatype.cc
    spectests/dynsegymd.cc
    spectests/file.cc
    spectests/filesegyintegread.cc
    spectests/filesegyintegwrite.cc
    spectests/filesegyread.cc
    spectests/filesegywrite.cc
    spectests/main.cc
    spectests/objsegyintegread.cc
    spectests/objsegyintegwrite.cc
    spectests/objsegymeta.cc
    spectests/objsegyread.cc
    spectests/objsegywrite.cc
    spectests/opstest.cc
    spectests/segy.cc
    spectests/segymd.cc
    spectests/segymdextra.cc
    spectests/selftests.cc
    spectests/set.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)
target_link_libraries(spectests exseisdat)


add_library(
    wraptests

    wraptests/googletest_variable_instances.cc

    wraptests/intercepted_c_bindings.cc

    wraptests/checkreturnlistener.cc
    wraptests/exseis.cc
    wraptests/exseiswraptest.cc
    wraptests/getminmax.cc
    wraptests/getminmaxwraptests.cc
    wraptests/mockexseis.cc
    wraptests/mockgetminmax.cc
    wraptests/mockparam.cc
    wraptests/mockreaddirect.cc
    wraptests/mockrule.cc
    wraptests/mockset.cc
    wraptests/mockwritedirect.cc
    wraptests/param.cc
    wraptests/paramwraptests.cc
    wraptests/printers.cc
    wraptests/readdirect.cc
    wraptests/readdirectwraptests.cc
    wraptests/rule.cc
    wraptests/rulewraptests.cc
    wraptests/set.cc
    wraptests/setwraptests.cc
    wraptests/wraptests.cc
    wraptests/wraptesttools.cc
    wraptests/writedirect.cc
    wraptests/writedirectwraptests.cc

    ${GOOGLETEST_DIR}/googletest/src/gtest-all.cc
    ${GOOGLETEST_DIR}/googlemock/src/gmock-all.cc
)
target_link_libraries(wraptests exseisdat)

add_executable(c_wraptest cwraptests/main.c)
target_link_libraries(c_wraptest wraptests)



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

# Define spectests here with the *Farm* tests disabled
add_test(
    NAME spectests_test
    #COMMAND spectests --gtest_filter=-*Farm* --gtest_filter=*IBM*
    COMMAND spectests --gtest_filter=-*Farm*
)

add_test(
    NAME c_wraptest_test
    COMMAND c_wraptest
)
