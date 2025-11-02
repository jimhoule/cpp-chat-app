set(GUI_LIB_NAME Gui)
set(GUI_CORE_NAME Core)
set(IMGUI_VENDOR_NAME ImGui)

add_library(${GUI_LIB_NAME} STATIC src/gui.cpp)
target_link_libraries(${GUI_LIB_NAME} PRIVATE ${GUI_CORE_NAME} ${IMGUI_VENDOR_NAME})
target_include_directories(${GUI_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# NOTE: Temporary until abtraction of glfw windows is created
target_include_directories(${GUI_LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw/include)
