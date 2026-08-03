set(CORE_LIB_NAME Core)
set(EPOLLSHIM_VENDOR_NAME epoll-shim)
set(GLAD_VENDOR_NAME Glad)
set(NLOHMANNJSON_VENDOR_NAME nlohmann_json::nlohmann_json)
set(STB_VENDOR_NAME Stb)

file(GLOB DESERIALIZER_SOURCE_FILES "src/deserializer/*.cpp")
file(GLOB JSON_SOURCE_FILES "src/json/*.cpp")
file(GLOB LAYER_SOURCE_FILES "src/layer/*.cpp")
file(GLOB LOG_SOURCE_FILES "src/log/*.cpp")
file(GLOB SERIALIZER_SOURCE_FILES "src/serializer/*.cpp")
file(GLOB SOCKET_SOURCE_FILES "src/socket/*.cpp")

add_library(${CORE_LIB_NAME} STATIC
    ${DESERIALIZER_SOURCE_FILES}
    ${JSON_SOURCE_FILES}
    ${LAYER_SOURCE_FILES}
    ${LOG_SOURCE_FILES}
    ${SERIALIZER_SOURCE_FILES}
    ${SOCKET_SOURCE_FILES}
    src/Texture.cpp
)
# NOTE: PUBLIC because nlohmann appears in Core's public headers, so consumers need its include path
target_link_libraries(${CORE_LIB_NAME} PUBLIC
    ${NLOHMANNJSON_VENDOR_NAME}
)
target_link_libraries(${CORE_LIB_NAME} PRIVATE
    ${GLAD_VENDOR_NAME}
    ${STB_VENDOR_NAME}
)
target_include_directories(${CORE_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(APPLE)
    message(STATUS "Setting MacOS epoll-shim to emulate epoll (only available in Linux)")
    target_link_libraries(${CORE_LIB_NAME} PRIVATE ${EPOLLSHIM_VENDOR_NAME})
endif()
