cmake_minimum_required(VERSION 3.20)
project(cxx-record)

set(CMAKE_CXX_STANDARD 17)
set(LLVM_CLANG_PACKAGE_DIR "/home/xq/Desktop/wasm-udf/wasi-sdk/build/llvm/")
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(LLVM REQUIRED CONFIG
    PATHS ${LLVM_CLANG_PACKAGE_DIR}
    NO_DEFAULT_PATH)

add_definitions(${LLVM_DEFNITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)

find_package(Clang REQUIRED CONFIG
    PATHS ${LLVM_CLANG_PACKAGE_DIR}
    NO_DEFAULT_PATH)
include_directories(${CLANG_INCLUDE_DIRS})
set(CLANG_LIB_DIR "/home/xq/Desktop/wasm-udf/wasi-sdk/src/llvm-project/clang/lib/")
include_directories(${CLANG_LIB_DIR})

# Handle RTTI stuff, which often leads to error
if(NOT ${LLVM_ENABLE_RTTI})
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
      "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
      "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    message(STATUS "Disable RTTI")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    message(STATUS "Disable RTTI")
  endif()
endif()

set(_SOURCE_FILES RecordDecl.cpp)
add_llvm_library(RecordDeclPlugin MODULE
    ${_SOURCE_FILES} PLUGIN_TOOL clang)

