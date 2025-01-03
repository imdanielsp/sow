find_package(GTest REQUIRED)

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
