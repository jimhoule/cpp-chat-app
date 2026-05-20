set(STB_VENDOR_NAME Stb)

add_library(${STB_VENDOR_NAME} INTERFACE)
# NOTE: SYSTEM keyword makes CMake emit -isystem /path/to/vendor/stb instead of -I so both the compiler and clang-tidy unconditionally suppress all diagnostics from system-included headers
target_include_directories(${STB_VENDOR_NAME} SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
