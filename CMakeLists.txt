cmake_minimum_required(VERSION 3.24)

project(emblec)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tree-sitter-emble)
add_subdirectory(vendor/tree-sitter)

add_executable(emblec 
    src/main.cpp
    vendor/tree-sitter/src/lib.c)
target_include_directories(emblec PRIVATE vendor/tree-sitter/include)
target_include_directories(emblec PRIVATE emblec)
target_link_libraries(emblec PRIVATE tree-sitter)
target_link_libraries(emblec PRIVATE tree-sitter-emble)
