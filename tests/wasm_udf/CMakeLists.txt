# Copyright (C) 2022 Intel Corporation.  All rights reserved.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.5)
project(c-wasm)


set (CMAKE_SYSTEM_PROCESSOR wasm32)


if (NOT DEFINED WASI_SDK_DIR)
    set (WASI_SDK_DIR               "/home/xiaqian/wasm-udf/wasi-sdk")    # use wasi-sdk path
endif ()

set (CMAKE_CXX_COMPILER_TARGET        "wasm32-wasi")
set (CMAKE_CXX_COMPILER               "/home/xiaqian/wasm-udf/wasi-sdk/build/install/opt/wasi-sdk/bin/clang++")
set (CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -O3 -z stack-size=409600000 -Wl,--initial-memory=2147483648")
set (CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -Wl,--allow-undefined")   # allow undefiend function
set (CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} --sysroot=/home/xiaqian/wasm-udf/wasi-sdk/build/install/opt/wasi-sdk/share/wasi-sysroot")
set (CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -L/home/xiaqian/wasm-udf/wasi-sdk/build/install/opt/wasi-sdk/share/wasi-sysroot/lib/wasm32-wasi")
set (CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -lc++abi -lc++")
include_directories("/home/xiaqian/wasm-udf/performance/flatbuffer/include")
#include_directories("/home/xiaqian/MITIE/mitielib/include")
#include_directories("/home/xiaqian/MITIE/dlib")
#include_directories("/home/xiaqian/wasm-udf/performance/wasm/include")
#find_library(flatcc libflatcc.a /home/xiaqian/wasm-udf/performance/flatbuffer/lib)
#find_library(icuuc libicuuc.a /home/xiaqian/wasm-udf/performance/wasm/lib)
#find_library(icuio libicuio.a /home/xiaqian/wasm-udf/performance/wasm/lib)
#find_library(mitie libmitie.a /home/xiaqian/MITIE/mitielib/build)
#find_library(dlib libdlib.a /home/xiaqian/MITIE/mitielib/build/dlib_build)
find_library(c++ libc++.a /home/xiaqian/wasm-udf/wasi-sdk/build/install/opt/wasi-sdk/share/wasi-sysroot/lib/wasm32-wasi)
find_library(c++abi libc++abi.a /home/xiaqian/wasm-udf/wasi-sdk/build/install/opt/wasi-sdk/share/wasi-sysroot/lib/wasm32-wasi)
#set (CMAKE_CXX_FLAGS                 "${CMAKE_CXX_FLAGS} -L${flatcc}")


file(GLOB CPP_FILES *.cpp)
foreach(FILE_PATH ${CPP_FILES})
    get_filename_component(FILE_NAME ${FILE_PATH} NAME_WE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export=${FILE_NAME}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export=${FILE_NAME}_with_data_prepared")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export=${FILE_NAME}_serialize_with_flatbuf")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export=${FILE_NAME}_with_ser")


endforeach()

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/wasm")
foreach(FILE_PATH ${CPP_FILES})
    get_filename_component(EXE_NAME ${FILE_PATH} NAME_WE)
    add_executable(${EXE_NAME}.wasm "${EXE_NAME}.cpp")
    target_link_libraries(${EXE_NAME}.wasm )
    #target_link_libraries(${EXE_NAME}.wasm ${flatcc})
    #target_link_libraries(${EXE_NAME}.wasm ${icuuc})
    #target_link_libraries(${EXE_NAME}.wasm ${icuio})
    #target_link_libraries(${EXE_NAME}.wasm ${mitie})
    #target_link_libraries(${EXE_NAME}.wasm ${dlib})
    target_link_libraries(${EXE_NAME}.wasm c++)
    target_link_libraries(${EXE_NAME}.wasm c++abi)
endforeach()


# add_executable(testapp.wasm testapp.cpp ../src/struct.pb-c.cpp)
# target_link_libraries(testapp.wasm)




