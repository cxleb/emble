cmake_minimum_required(VERSION 3.24)

project(emblec)

# standard c++ cmake setup
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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
elseif(APPLE)
    find_program(LLVM_CONFIG_EXE NAMES llvm-config "/opt/homebrew/opt/llvm/bin/llvm-config")

    execute_process(
        COMMAND ${LLVM_CONFIG_EXE} --libs
        OUTPUT_VARIABLE LLVM_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND ${LLVM_CONFIG_EXE} --includedir
        OUTPUT_VARIABLE LLVM_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
        COMMAND ${LLVM_CONFIG_EXE} --libdir
        OUTPUT_VARIABLE LLVM_LIB_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    target_link_directories(emblec PRIVATE ${LLVM_LIB_DIR})
    target_link_libraries(emblec PRIVATE ${LLVM_LIBRARIES})
    target_include_directories(emblec PRIVATE ${LLVM_INCLUDE_DIR})
endif()