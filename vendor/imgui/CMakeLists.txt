set(IMGUI_VENDOR_NAME ImGui)

add_library(${IMGUI_VENDOR_NAME} STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    # imgui misc
    imgui/imgui_stdlib.cpp
    # imgui backends
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)
# NOTE: SYSTEM keyword makes CMake emit -isystem /path/to/vendor/stb instead of -I so both the compiler and clang-tidy unconditionally suppress all diagnostics from system-included headers
target_include_directories(${IMGUI_VENDOR_NAME} SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${IMGUI_VENDOR_NAME} SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
