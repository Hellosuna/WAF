最初版共享内存定制版clang：

 **辅助函数生成函数** 
在clang/lib/CodeGen/CodeGenModule.h中定义，并在clang/lib/CodeGen/CodeGenModule.cpp中实现函数EmitStructDroppedFunc

在clang/lib/CodeGen/CodeGenTypeCache.h中定义存储（insert_dropped_function）和查找函数(get_dropped_function)，使用STL的unorderd_map哈希表存储(key:stl string，value:llvm function*)

 **为每个结构体生成辅助函数** 
在clang/lib/CodeGen/ModuleBuilder.cpp的HandleTagDeclDefinition函数中生成。

```
       Builder->UpdateCompletedType(D);

+      if (RecordDecl *RD = dyn_cast<RecordDecl>(D))
+        Builder->EmitStructDroppedFunc(RD);
+
       // For MSVC compatibility, treat declarations of static data members with
       // inline initializers as definitions.

```


 **修改GEP命令调用辅助函数** 
在clang/lib/CodeGen/CGBuilder.h的CreateStructGEP函数修改。

```
 const llvm::DataLayout &DL = BB->getParent()->getParent()->getDataLayout();
     const llvm::StructLayout *Layout = DL.getStructLayout(ElTy);
     auto Offset = CharUnits::fromQuantity(Layout->getElementOffset(Index));
-
-    return Address(
-        CreateStructGEP(Addr.getElementType(), Addr.getPointer(), Index, Name),
-        ElTy->getElementType(Index),
-        Addr.getAlignment().alignmentAtOffset(Offset));
+    llvm::Function *dropped_func =
+        TypeCache.get_dropped_function(ElTy->getName());
+    llvm::Value *ptr =
+        CreateStructGEP(Addr.getElementType(), Addr.getPointer(), Index, Name);
+    if (!dropped_func) {
+      std::array<llvm::Value *, 2> argv_array{ptr, getInt32(Index)};
+      llvm::ArrayRef<llvm::Value *> argv(argv_array);
+      ptr = CreateCall(dropped_func->getFunctionType(), dropped_func);
+    }
+
+    return Address(ptr, ElTy->getElementType(Index),
+                   Addr.getAlignment().alignmentAtOffset(Offset));

```
