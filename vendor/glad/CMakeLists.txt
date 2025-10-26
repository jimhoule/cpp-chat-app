set(GLAD_VENDOR_NAME Glad)

add_library(${GLAD_VENDOR_NAME} STATIC src/glad.c)
target_include_directories(${GLAD_VENDOR_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
