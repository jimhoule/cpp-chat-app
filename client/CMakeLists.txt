set(CLIENT_APP_NAME Client)
set(CORE_LIB_NAME Core)

add_executable(${CLIENT_APP_NAME} src/client.cpp)

target_link_libraries(${CLIENT_APP_NAME} ${CORE_LIB_NAME})
