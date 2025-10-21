set(CORE_LIB_NAME Core)

add_library(${CORE_LIB_NAME} STATIC src/SocketServer.cpp src/SocketClient.cpp)

target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
