set(SERVER_APP_NAME Server)
set(CORE_LIB_NAME Core)

file(GLOB AUTH_SOURCE_FILES "src/auth/*.cpp")

add_executable(${SERVER_APP_NAME} ${AUTH_SOURCE_FILES} src/main.cpp)
target_link_libraries(${SERVER_APP_NAME} PRIVATE ${CORE_LIB_NAME})
target_include_directories(${SERVER_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
