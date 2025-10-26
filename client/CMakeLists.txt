set(CLIENT_APP_NAME Client)
set(CORE_LIB_NAME Core)
set(GLAD_VENDOR_NAME Glad)
set(GLFW_VENDOR_NAME glfw)
set(GLM_VENDOR_NAME glm::glm)
set(IMGUI_VENDOR_NAME ImGui)

add_executable(${CLIENT_APP_NAME} src/client.cpp)

target_link_libraries(${CLIENT_APP_NAME} ${CORE_LIB_NAME} ${GLAD_VENDOR_NAME} ${GLFW_VENDOR_NAME} ${GLM_VENDOR_NAME} ${IMGUI_VENDOR_NAME})
