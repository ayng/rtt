cmake_minimum_required(VERSION 3.1)

project(rtt)
add_executable(rtt main.cpp)

target_compile_options(rtt PRIVATE -Wall -Wextra -Wpedantic)
target_compile_features(rtt PUBLIC cxx_lambdas cxx_raw_string_literals)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.2.1)
target_link_libraries(rtt PRIVATE glfw)

set(GLAD_NO_LOADER ON)
add_subdirectory(glad-0.1.28)
target_link_libraries(rtt PRIVATE glad)
