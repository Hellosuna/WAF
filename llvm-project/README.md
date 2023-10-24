最初版共享内存定制版clang：
 **辅助函数生成函数** 
在clang/lib/CodeGen/CodeGenModule.h中定义，并在clang/lib/CodeGen/CodeGenModule.cpp中实现函数EmitStructDroppedFunc
在clang/lib/CodeGen/CodeGenTypeCache.h中定义存储和查找函数，使用STL的unorderedmap哈希表存储(key:stl string，value:llvm function*)

 **为每个结构体生成辅助函数** 
在clang/lib/CodeGen/ModuleBuilder.cpp的HandleTagDeclDefinition函数中生成。

 **修改GEP命令调用辅助函数** 
在clang/lib/CodeGen/CGBuilder.h的CreateStructGEP函数修改。