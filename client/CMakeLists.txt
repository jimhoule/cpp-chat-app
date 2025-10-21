set(CLIENT_APP_NAME Client)
set(CORE_LIB_NAME Core)
set(VENDOR_LIB_NAME Vendor)

add_executable(${CLIENT_APP_NAME} src/client.cpp)

target_link_libraries(${CLIENT_APP_NAME} ${CORE_LIB_NAME})
target_link_libraries(${CLIENT_APP_NAME} ${VENDOR_LIB_NAME})
