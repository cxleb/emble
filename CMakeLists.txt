cmake_minimum_required(VERSION 3.24)

project(emblec)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tree-sitter-emble)
add_subdirectory(vendor/tree-sitter)

add_executable(emblec 
    src/main.cpp
    src/ir.cpp
    src/irgen.cpp
    src/codegen.cpp
    vendor/tree-sitter/src/lib.c)
target_include_directories(emblec PRIVATE vendor/tree-sitter/include)
target_include_directories(emblec PRIVATE emblec)
target_link_libraries(emblec PRIVATE tree-sitter)
target_link_libraries(emblec PRIVATE tree-sitter-emble)

#llvm
# the expectation is on windows that llvm is installed to c:\llvm{major version}
if(WIN32)
    target_link_directories(emblec PRIVATE "c:\\llvm19\\lib")
    target_link_libraries(emblec PRIVATE LLVM-C)
    target_include_directories(emblec PRIVATE "c:\\llvm19\\include")
endif()