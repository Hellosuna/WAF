# Clang Record Plugin

## 1. replace package lib dir to yours
```
vim CMakeLists.txt

...
set(LLVM_CLANG_PACKAGE_DIR /path/to/build/llvm)   // llvm 编译后的目录 buil/llvm
...
set(CLANG_LIB_DIR /path/to/clang/lib)             // clang 源码 lib 目录
...
```

## 2. compile plugin
```
mkdir build
cd build
cmake ..
make
```

## 3. compile wasm with clang plugin
```
/path/to/clang++  \ 
-fplugin=/path/to/RecordPlugin/build/RecordDeclPlugin.so \  // 第二步中编译生成的插件路径
-Xclang -plugin \
-Xclang record \
--target=wasm32-wasi \
--sysroot=/path/to/wasi-sysroot \
file.cpp  
```

