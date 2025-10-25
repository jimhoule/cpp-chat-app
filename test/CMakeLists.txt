enable_testing()
# NOTE: Prevents overriding the parent project's compiler/linker settings for Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(TEST_APP_NAME Test)
set(TEST_DEPENDENCY_NAME GoogleTest)

include(FetchContent)
FetchContent_Declare(
  ${TEST_DEPENDENCY_NAME}
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(${TEST_DEPENDENCY_NAME})

add_executable(${TEST_APP_NAME} src/example.cpp)
target_link_libraries(${TEST_APP_NAME} PRIVATE gtest_main gmock_main)

include(GoogleTest)
gtest_discover_tests(${TEST_APP_NAME})
