cmake_minimum_required(VERSION 3.24)

project(emblec)

# standard c++ cmake setup
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_executable(emblec 
    src/main.cpp
    src/utils.cpp
    src/error.cpp
    src/lexer.cpp
    src/ast.cpp
    src/ir.cpp
    src/irgen.cpp
    src/codegen.cpp
    vendor/tree-sitter/src/lib.c
    tree-sitter-emble/src/parser.c)
target_include_directories(emblec PRIVATE vendor/tree-sitter/include)
target_include_directories(emblec PRIVATE tree-sitter-emble/src/src)
target_include_directories(emblec PRIVATE emblec)

# llvm
find_program(LLVM_CONFIG_EXE NAMES llvm-config "/opt/homebrew/opt/llvm/bin/llvm-config")

# On windows, we need to know if we are linking with the debug or release version of the runtime
if(WIN32)
    execute_process(
        COMMAND ${LLVM_CONFIG_EXE} --build-mode
        OUTPUT_VARIABLE LLVM_BUILD_MODE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(TOLOWER ${LLVM_BUILD_MODE} LLVM_BUILD_MODE)
    IF(LLVM_BUILD_MODE STREQUAL "release")
        set_property(TARGET emblec PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    ENDIF()
ENDIF()

execute_process(
    COMMAND ${LLVM_CONFIG_EXE} --libnames
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    
execute_process(
    COMMAND ${LLVM_CONFIG_EXE} --system-libs
    OUTPUT_VARIABLE LLVM_SYS_LIBS
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
target_link_libraries(emblec PRIVATE ${LLVM_LIBS} ${LLVM_SYS_LIBS})
target_include_directories(emblec PRIVATE ${LLVM_INCLUDE_DIR})

#####################
# Build Tests

if(BUILD_TESTS)
    include(CTest)
    add_executable(emble-test-lexer tests/test_lexer.cpp 
        src/utils.cpp
        src/error.cpp
        src/lexer.cpp
    )
    target_include_directories(emble-test-lexer PRIVATE src)
    add_test(
        NAME emble-test-lexer
        COMMAND emble-test-lexer
    )
    add_executable(emble-test-parser tests/test_parser.cpp 
        src/utils.cpp
        src/error.cpp
        src/lexer.cpp
        src/parser.cpp
        src/ast.cpp
    )
    target_include_directories(emble-test-parser PRIVATE src)
    add_test(
        NAME emble-test-parser
        COMMAND emble-test-parser
    )
ENDIF()