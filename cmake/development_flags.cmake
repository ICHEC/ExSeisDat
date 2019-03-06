add_library(development_flags INTERFACE)

if(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)

option(EXSEISDAT_ENABLE_SANITIZERS "Enable ASAN and UBSAN." ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(
        development_flags
        INTERFACE
            -Wall -Wextra -Wpedantic -pedantic
            -Wno-error=unknown-pragmas
            -Wswitch-enum
            -Wimplicit-fallthrough
            -Werror
            -Og
    )

    if(EXSEISDAT_ENABLE_SANITIZERS)
        target_compile_options(
            development_flags
            INTERFACE
                -fsanitize=address,undefined
                -fno-sanitize-recover=all
                -fno-omit-frame-pointer
        )
        target_link_libraries(
            development_flags
            INTERFACE
                -fsanitize=address,undefined
        )
    endif(EXSEISDAT_ENABLE_SANITIZERS)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(development_flags INTERFACE -ferror-limit=1)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(development_flags INTERFACE -fmax-errors=1)
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

endif(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)
