set(GLAD_SUBDIRECTORY_NAME glad)
set(GLFW_SUBDIRECTORY_NAME glfw)
set(GLM_SUBDIRECTORY_NAME glm)
set(IMGUI_SUBDIRECTORY_NAME imgui)
set(NLOHMANNJSON_SUBDIRECTORY_NAME nlohmannjson)
set(STB_SUBDIRECTORY_NAME stb)

# glad
add_subdirectory(${GLAD_SUBDIRECTORY_NAME})

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_SUBDIRECTORY_NAME})

# glm
add_subdirectory(${GLM_SUBDIRECTORY_NAME})

# imgui
add_subdirectory(${IMGUI_SUBDIRECTORY_NAME})

# nlohmann json
set(JSON_BuildTests OFF CACHE INTERNAL "")
# NOTE: If you only include this third party in PRIVATE source files, you do not need to install it when your main project gets installed
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(${NLOHMANNJSON_SUBDIRECTORY_NAME})

#stb
add_subdirectory(${STB_SUBDIRECTORY_NAME})
