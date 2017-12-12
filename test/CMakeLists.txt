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

add_test(
    NAME mkdir_tmp
    COMMAND mkdir -p tmp
)
add_test(
    NAME genfiles_test
    COMMAND genfiles
)
set_tests_properties(genfiles_test PROPERTIES DEPENDS mkdir_tmp)

add_test(
    NAME spectests_test
    COMMAND spectests --gtest_filter=-*Farm*
)
set_tests_properties(spectests_test PROPERTIES DEPENDS genfiles_test)
