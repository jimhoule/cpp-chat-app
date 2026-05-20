set(GLAD_VENDOR_NAME Glad)

add_library(${GLAD_VENDOR_NAME} STATIC src/glad.c)
# NOTE: SYSTEM keyword makes CMake emit -isystem /path/to/vendor/stb instead of -I so both the compiler and clang-tidy unconditionally suppress all diagnostics from system-included headers
target_include_directories(${GLAD_VENDOR_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
