add_library(development_flags INTERFACE)

include(CheckCXXCompilerFlag)
include(CMakePushCheckState)

if(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)

option(EXSEISDAT_ENABLE_SANITIZERS "Enable ASAN and UBSAN." ON)


# Try and add the compiler option to development_flags
function(try_add_compile_option option test_name)
    cmake_push_check_state()

    # Add -Werror to catch warnings about flags
    check_cxx_compiler_flag(-Werror Werror)
    if(Werror)
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
    endif(Werror)

    check_cxx_compiler_flag(${option} ${test_name})
    if(${test_name})
        target_compile_options(development_flags INTERFACE ${option})
    endif(${test_name})

    cmake_pop_check_state()
endfunction(try_add_compile_option)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    try_add_compile_option(-Wall Wall)
    try_add_compile_option(-Wextra Wextra)
    try_add_compile_option(-Wpedantic Wpedantic)
    try_add_compile_option(-pedantic pedantic)
    try_add_compile_option(-Wno-error=unknown-pragmas Wno_error_unknown_pragmas)
    try_add_compile_option(-Wswitch-enum Wswitch_enum)
    try_add_compile_option(-Wimplicit-fallthrough Wimplicit_fallthrough)
    try_add_compile_option(-Werror Werror)
    try_add_compile_option(-Og Og)
    try_add_compile_option(-g g)

    # Try adding error limit, different for clang and gcc
    try_add_compile_option(-ferror-limit=1 ferror_limit_1)
    try_add_compile_option(-fmax-errors=1 fmax_errors_1)


    if(EXSEISDAT_ENABLE_SANITIZERS)
        try_add_compile_option(-fsanitize=address,undefined fsanitize_address_undefined)
        try_add_compile_option(-fno-sanitize-recover=all fno_sanitize_recover_all)
        try_add_compile_option(-fno-omit-frame-pointer fno_omit_frame_pointer)
        target_link_libraries(
            development_flags
            INTERFACE
                -fsanitize=address,undefined
        )
    endif(EXSEISDAT_ENABLE_SANITIZERS)
endif()

endif(EXSEISDAT_ENABLE_DEVELOPMENT_FLAGS)
