cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# NOTE: For clang-tidy linter
set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
	message(STATUS "Setting MacOS environment")
	set(CMAKE_C_COMPILER /usr/bin/clang)
	set(CMAKE_CXX_COMPILER /usr/bin/clang++)
	set(CMAKE_SYSTEM_NAME Darwin)

	find_program(XCRUN_PATH xcrun)
	execute_process(COMMAND ${XCRUN_PATH} --show-sdk-path OUTPUT_VARIABLE MACOS_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
elseif(UNIX)
	message(STATUS "Setting Linux environment")
	set(CMAKE_C_COMPILER /usr/bin/clang)
	set(CMAKE_CXX_COMPILER /usr/bin/clang++)
	set(CMAKE_SYSTEM_NAME Linux)
	set(CMAKE_OSX_SYSROOT "/")
endif()

set(PROJECT_NAME Chat)
set(CORE_SUBDIRECTORY_NAME core)
set(VENDOR_SUBDIRECTORY_NAME vendor)
set(TEST_SUBDIRECTORY_NAME test)
set(CLIENT_SUBDIRECTORY_NAME client)
set(SERVER_SUBDIRECTORY_NAME server)

project(${PROJECT_NAME})

add_subdirectory(${CORE_SUBDIRECTORY_NAME})
add_subdirectory(${VENDOR_SUBDIRECTORY_NAME})
add_subdirectory(${TEST_SUBDIRECTORY_NAME})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT_SUBDIRECTORY_NAME}")
    message(STATUS "Adding ${CLIENT_SUBDIRECTORY_NAME} subdirectory")
    add_subdirectory(${CLIENT_SUBDIRECTORY_NAME})
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SERVER_SUBDIRECTORY_NAME}")
    message(STATUS "Adding ${SERVER_SUBDIRECTORY_NAME} subdirectory")
    add_subdirectory(${SERVER_SUBDIRECTORY_NAME})
endif()
