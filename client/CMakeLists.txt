set(CLIENT_APP_NAME Client)
set(CORE_LIB_NAME Core)
set(GUI_LIB_NAME Gui)
set(GLAD_VENDOR_NAME Glad)
set(GLFW_VENDOR_NAME glfw)
set(GLM_VENDOR_NAME glm::glm)
set(IMGUI_VENDOR_NAME ImGui)

if(APPLE)
    set(APP_DATA_HOME_VARIABLE "HOME")
    set(APP_DATA_DIRECTORY "Library/Application Support")
elseif(WIN32)
    # NOTE: APPDATA is already the full path so there is no sub directory to append
    set(APP_DATA_HOME_VARIABLE_NAME "APPDATA")
    set(APP_DATA_DIRECTORY "")
elseif(UNIX)
    # NOTE: APPDATA is already the full path so there is no sub directory to append
    set(APP_DATA_HOME_VARIABLE_NAME "HOME")
    set(APP_DATA_DIRECTORY ".config")
endif()

file(GLOB AUTH_SOURCE_FILES "src/auth/*.cpp")
file(GLOB DEBUG_SOURCE_FILES "src/debug/*.cpp")
file(GLOB LAYER_SOURCE_FILES "src/layer/*.cpp")
file(GLOB MESSAGES_SOURCE_FILES "src/messages/*.cpp")
file(GLOB NAVIGATION_SOURCE_FILES "src/navigation/*.cpp")
file(GLOB SESSIONS_SOURCE_FILES "src/sessions/*.cpp")

add_executable(${CLIENT_APP_NAME} ${AUTH_SOURCE_FILES} ${DEBUG_SOURCE_FILES} ${LAYER_SOURCE_FILES} ${MESSAGES_SOURCE_FILES} ${NAVIGATION_SOURCE_FILES} ${SESSIONS_SOURCE_FILES} src/main.cpp)
target_link_libraries(${CLIENT_APP_NAME} PRIVATE
    ${CORE_LIB_NAME}
    ${GUI_LIB_NAME}
    ${GLAD_VENDOR_NAME}
    ${GLFW_VENDOR_NAME}
    ${GLM_VENDOR_NAME}
    ${IMGUI_VENDOR_NAME}
)
target_include_directories(${CLIENT_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# NOTE: $<CONFIG> resolves at build time unlike CMAKE_BUILD_TYPE which is empty on multi config generators like Xcode
target_compile_definitions(${CLIENT_APP_NAME} PRIVATE
    APP_NAME="${PROJECT_NAME}"
    APP_BUILD_TYPE="$<CONFIG>"
    APP_DATA_HOME_VARIABLE="${APP_DATA_HOME_VARIABLE}"
    APP_DATA_DIRECTORY="${APP_DATA_DIRECTORY}"
)
