include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

add_executable(
    sow-test
    sow-test.cpp
)

target_include_directories(
    sow-test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    sow-test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sow-test)
