set(CLIENT_APP_NAME Client)
set(CORE_LIB_NAME Core)
set(GUI_LIB_NAME Gui)
set(GLAD_VENDOR_NAME Glad)
set(GLFW_VENDOR_NAME glfw)
set(GLM_VENDOR_NAME glm::glm)
set(IMGUI_VENDOR_NAME ImGui)

file(GLOB API_SOURCE_FILES "src/api/*.cpp")
file(GLOB DEBUG_SOURCE_FILES "src/debug/*.cpp")
file(GLOB LAYER_SOURCE_FILES "src/layer/*.cpp")

add_executable(${CLIENT_APP_NAME} ${API_SOURCE_FILES} ${DEBUG_SOURCE_FILES} ${LAYER_SOURCE_FILES} src/main.cpp)
target_link_libraries(${CLIENT_APP_NAME} PRIVATE
    ${CORE_LIB_NAME}
    ${GUI_LIB_NAME}
    ${GLAD_VENDOR_NAME}
    ${GLFW_VENDOR_NAME}
    ${GLM_VENDOR_NAME}
    ${IMGUI_VENDOR_NAME}
)
target_include_directories(${CLIENT_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
