# Building tests requires the googletest source in a test/googletest directory!
# To get googletest, https://github.com/google/googletest/
# Quickly:
#   from the exseisdat project directory:
#   cd test && git clone https://github.com/google/googletest.git

# Link all the libraries with exseisdat
link_libraries(exseisdat)

# Include googletest directories.
include_directories(
    googletest/googletest/include
    googletest/googletest
    googletest/googlemock/include
    googletest/googlemock
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

    googletest/googletest/src/gtest-all.cc
    googletest/googlemock/src/gmock-all.cc
)

add_test(
    NAME mkdir_tmp
    COMMAND mkdir -p tmp
)
add_test(
    NAME genfiles_test
    COMMAND genfiles
    #COMMAND echo genfiles
)
set_tests_properties(genfiles_test PROPERTIES DEPENDS mkdir_tmp)

add_test(
    NAME spectests_test
    COMMAND spectests --gtest_filter=-*Farm*
)
set_tests_properties(spectests_test PROPERTIES DEPENDS genfiles_test)
