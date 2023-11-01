 **环境搭建** 
主要需要两个库：wasi-sdk和wamr

1. wasi-sdk:
https://github.com/WebAssembly/wasi-sdk
如果只配环境可以直接用 https://github.com/WebAssembly/wasi-sdk#install 安装，不用build，缺点就是没有源码。

2.wamr：
https://github.com/bytecodealliance/wasm-micro-runtime
跟着教程走就行，VMCore和iwasm是必须安装的，wamrc是用来编译.aot，建议安装。