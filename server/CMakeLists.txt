set(SERVER_APP_NAME Server)
set(CORE_LIB_NAME Core)

file(GLOB_RECURSE AUTH_SOURCE_FILES "src/auth/*.cpp")
file(GLOB_RECURSE MESSAGES_SOURCE_FILES "src/messages/*.cpp")
file(GLOB_RECURSE USERS_SOURCE_FILES "src/users/*.cpp")

add_executable(${SERVER_APP_NAME} ${AUTH_SOURCE_FILES} ${MESSAGES_SOURCE_FILES} ${USERS_SOURCE_FILES} src/main.cpp)
target_link_libraries(${SERVER_APP_NAME} PRIVATE ${CORE_LIB_NAME})
target_include_directories(${SERVER_APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
