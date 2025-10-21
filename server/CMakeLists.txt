set(SERVER_APP_NAME Server)
set(CORE_LIB_NAME Core)

add_executable(${SERVER_APP_NAME} src/server.cpp)

target_link_libraries(${SERVER_APP_NAME} ${CORE_LIB_NAME})
