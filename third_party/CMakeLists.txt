add_library(bundled_CLI11 INTERFACE)
target_include_directories(bundled_CLI11 SYSTEM INTERFACE CLI11-v1.7.1)
add_library(CLI11::CLI11 ALIAS bundled_CLI11)
