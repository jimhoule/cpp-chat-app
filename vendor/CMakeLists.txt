set(VENDOR_LIB_NAME Vendor)

add_library(${VENDOR_LIB_NAME} INTERFACE)

target_include_directories(${VENDOR_LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
