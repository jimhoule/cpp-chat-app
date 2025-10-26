set(IMGUI_VENDOR_NAME ImGui)

add_library(${IMGUI_VENDOR_NAME} STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    # imgui backends
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)
target_include_directories(${IMGUI_VENDOR_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
target_include_directories(${IMGUI_VENDOR_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
