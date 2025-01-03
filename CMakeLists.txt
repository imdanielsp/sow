cmake_minimum_required(VERSION 3.16)
project(
    SOW
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

# Enable warnings and treat them as errors
add_compile_options(-Wall -Wextra -pedantic -Werror)

# Add the library
add_library(SOW INTERFACE)
target_include_directories(SOW INTERFACE include/)

# Option to enable test
option(BUILD_TEST "Build the test" ON)

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

install(
    DIRECTORY include/
    DESTINATION include
)
