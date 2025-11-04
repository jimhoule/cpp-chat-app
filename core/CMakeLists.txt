set(CORE_LIB_NAME Core)
set(GLAD_VENDOR_NAME Glad)
set(STB_VENDOR_NAME Stb)

add_library(${CORE_LIB_NAME} STATIC src/SocketServer.cpp src/SocketClient.cpp src/Texture.cpp)
target_link_libraries(${CORE_LIB_NAME} PRIVATE ${GLAD_VENDOR_NAME} ${STB_VENDOR_NAME})
target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
