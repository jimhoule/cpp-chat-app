set(EPOLLSHIM_SUBDIRECTORY_NAME epollshim)
set(GLAD_SUBDIRECTORY_NAME glad)
set(GLFW_SUBDIRECTORY_NAME glfw)
set(GLM_SUBDIRECTORY_NAME glm)
set(IMGUI_SUBDIRECTORY_NAME imgui)
set(LIBSODIUM_SUBDIRECTORY_NAME libsodium)
set(NLOHMANNJSON_SUBDIRECTORY_NAME nlohmannjson)
set(STB_SUBDIRECTORY_NAME stb)
set(STDUUID_SUBDIRECTORY_NAME stduuid)

# epoll-shim
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(ENABLE_COMPILER_WARNINGS OFF CACHE INTERNAL "")
add_subdirectory(${EPOLLSHIM_SUBDIRECTORY_NAME})

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

# libsodium
# NOTE: Upstream libsodium has no CMake support, this submodule is the robinlinden/libsodium-cmake wrapper which vendors jedisct1/libsodium itself as a nested submodule
set(SODIUM_DISABLE_TESTS ON CACHE INTERNAL "")
# NOTE: Keeps the full API available, the minimal build drops sealed boxes, generichash and pwhash
set(SODIUM_MINIMAL OFF CACHE INTERNAL "")
# NOTE: SYSTEM keyword propagates -isystem to consumers so clang-tidy stays quiet on sodium headers
add_subdirectory(${LIBSODIUM_SUBDIRECTORY_NAME} SYSTEM)

# nlohmann json
set(JSON_BuildTests OFF CACHE INTERNAL "")
# NOTE: If you only include this third party in PRIVATE source files, you do not need to install it when your main project gets installed
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(${NLOHMANNJSON_SUBDIRECTORY_NAME})

# stb
add_subdirectory(${STB_SUBDIRECTORY_NAME})

# stduuid
set(UUID_BUILD_TESTS OFF CACHE INTERNAL "")
# NOTE: Same reason as nlohmann json, this third party only appears in PRIVATE source files so it does not need to be installed with the project
set(UUID_ENABLE_INSTALL OFF CACHE INTERNAL "")
# NOTE: Stays OFF because the project is C++17, stduuid then uses the gsl span implementation it vendors itself
set(UUID_USING_CXX20_SPAN OFF CACHE INTERNAL "")
add_subdirectory(${STDUUID_SUBDIRECTORY_NAME})
