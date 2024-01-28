#pragma once
#include "clang/AST/Decl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DataLayout.h"

#include <unordered_set>

namespace clang {

class RecordRevise{

private:
    // CodeGen::CodeGenTypes &cgt;
    // llvm::LLVMContext &ctx;   
    // std::unordered_map<std::string, std::vector<int>> recordOffsets;
    // std::unordered_map<std::string, int> recordArrayOffsets;
    std::unordered_set<RecordDecl *> paramRecords;
    std::unordered_map<FieldDecl *, ValueDecl *> unionFieldMap;
    std::unordered_set<RecordDecl*> unionRecords;

    void modifyPointerToUnion(RecordDecl* RD){
        if(unionRecords.count(RD))
            return;

        std::vector<FieldDecl*> fields;

        for(auto it = RD->field_begin(); it != RD->field_end(); ++it){
            // llvm::outs() << "field " << it->getNameAsString() << " type: "
            //      << it->getType().getAsString() << "\n";
            fields.push_back(*it);
        }
        for(auto field : fields){
            RD->removeDecl(field);
        }
        for(auto field : fields){
            auto *type = field->getType().getTypePtr();
            if(type->isPointerType()){
                auto *Field = createUnionField(field);
                RD->addDecl(Field);
                unionFieldMap.insert(std::make_pair(field, Field));
                continue;
            }else if(auto *constantArray = dyn_cast<ConstantArrayType>(type)){
                if(constantArray->getBaseElementTypeUnsafe()->isPointerType()){
                    auto *Field = createUnionArrayField(field);
                    RD->addDecl(Field);
                    unionFieldMap.insert(std::make_pair(field, Field));
                    continue;
                }
            }
            RD->addDecl(field);
        }
    }

    void revise1(RecordDecl* RD){
        auto &context = RD->getASTContext();
        paramRecords.insert(RD);
        modifyPointerToUnion(RD);

        // QualType longIntType = context.getIntTypeForBitwidth(64,1);
        QualType longIntType = context.LongLongTy;

        for(auto it = RD->field_begin(); it != RD->field_end(); ++it){
            QualType field_qual_type = it->getType().getCanonicalType().getUnqualifiedType();
            const Type *field_type = it->getType().getTypePtr();
            if(field_type->isIntegerType() && field_qual_type.getAsString() == "long"){
                it->setType(longIntType);
                continue;
            }else if(field_type->isEnumeralType()){
                // 应该不需要处li?
                continue;
            }else if(field_type->isArrayType()){
                // don't use context.getBaseElementTpye. it represents the most inner type and ignore embeded array.
                // QualType element_qual_type = context.getBaseElementType(it->getType());
                QualType base_element_type = context.getBaseElementType(it->getType());
                if(base_element_type.getTypePtr()->isPointerType()){
                    // EmitArrayOffsetFunc("ptrArrayOffset", 4);
                    // longSign.push_back(false);
                    continue;
                }
                if(base_element_type.getAsString() == "long"){
                    auto arrayType = context.getAsConstantArrayType(field_qual_type);
                    QualType element_qual_type = arrayType->getElementType();
                    
                    std::vector<unsigned> dimen(1, context.getConstantArrayElementCount(arrayType)); // the size for dimension, the index value for counts
                    // because context.getConstantArrayElementCount counts the total element, so only the last dimension is true
                    // and use the previous value to divide next value the get the actual counts.
                    
                    while(element_qual_type.getTypePtr()->isArrayType()){
                        arrayType = context.getAsConstantArrayType(element_qual_type);
                        element_qual_type = arrayType->getElementType();
                        dimen.push_back(context.getConstantArrayElementCount(arrayType));
                    }

                    for(int i=0; i<dimen.size()-1; ++i){
                        dimen[i] = dimen[i] / dimen[i+1];
                    }

                    QualType newArrayType = context.getConstantArrayType(longIntType, llvm::APInt(32, dimen[dimen.size()-1]), nullptr, ArraySizeModifier::Normal, 0);
                    for(int i=dimen.size()-2; i>=0; --i){
                        newArrayType = context.getConstantArrayType(newArrayType, llvm::APInt(32,dimen[i]), nullptr, ArraySizeModifier::Normal, 0);
                    }
                    // 设置数组类型
                    it->setType(newArrayType);
                    continue;
                }
                if(base_element_type->isRecordType()){
                    revise(base_element_type->getAsRecordDecl());
                }
            }else if(field_type->isRecordType()){   // contain struct, class and union
                revise(field_type->getAsRecordDecl());
            }
        }
    }

public:
    // RecordRevise(llvm::LLVMContext &context, CodeGen::CodeGenTypes &CGT)
    //     :ctx(context), cgt(CGT){}
    RecordRevise(){}

    // delete this constructor to avoid some code bug: such we want to use a ref but ignored '&'
    RecordRevise(const RecordRevise &RecordRevise) = delete;
    void operator=(const RecordRevise &RecordRevise) = delete;

    FieldDecl *createField(FieldDecl *FD, QualType T, std::string name){
        auto &context = FD->getASTContext();
        SourceLocation StartLoc = FD->getBeginLoc();
        SourceLocation IdLoc = FD->getEndLoc();
        IdentifierTable &identTable = context.Idents;
        IdentifierInfo *fieldName = &identTable.get(name);
        TypeSourceInfo *TInfo = context.CreateTypeSourceInfo(T);
        Expr *BW = nullptr;
        bool Mutable = false;
        InClassInitStyle InitStyle = ICIS_ListInit;
        return FieldDecl::Create(context, nullptr, StartLoc, IdLoc, fieldName
            ,T,TInfo,BW,Mutable,InitStyle);
    }

    RecordDecl *createUnionDecl(FieldDecl *FD){
        auto &context = FD->getASTContext();
        DeclContext *DC = FD->getParent();
        SourceLocation StartLoc = FD->getBeginLoc();
        SourceLocation IdLoc = FD->getEndLoc();
        IdentifierTable &identTable = context.Idents;
        IdentifierInfo *unionName = &identTable.get("union_" + FD->getNameAsString());
        CXXRecordDecl *unionDecl = CXXRecordDecl::Create(context,TagTypeKind::Union, DC, StartLoc, IdLoc,
            unionName, nullptr, false);
        unionRecords.insert(unionDecl);
        return unionDecl;
    }

    FieldDecl *createUnionField(FieldDecl *FD){
        auto &context = FD->getASTContext();
        DeclContext *DC = FD->getParent();
        SourceLocation StartLoc = FD->getBeginLoc();
        SourceLocation IdLoc = FD->getEndLoc();
        IdentifierTable &identTable = context.Idents;
        IdentifierInfo *unionName = &identTable.get("union_" + FD->getNameAsString());
        CXXRecordDecl *unionDecl = CXXRecordDecl::Create(context,TagTypeKind::Union, DC, StartLoc, IdLoc,
            unionName, nullptr, false);
        unionRecords.insert(unionDecl);
        
        // add union field
        unionDecl->startDefinition();   // note: must call startDefinition
        FD->setDeclContext(unionDecl);
        unionDecl->addDecl(FD);
        FieldDecl* LLField = createField(FD, context.LongLongTy, "padding");
        LLField->setDeclContext(unionDecl);
        unionDecl->addDecl(LLField);
        unionDecl->completeDefinition();
        unionDecl->setCompleteDefinition();
        DC->addDecl(unionDecl);

        IdentifierInfo *fieldName = &identTable.get(FD->getNameAsString());
        QualType unionType = QualType(unionDecl->getTypeForDecl(), 0);
        TypeSourceInfo *TInfo = context.CreateTypeSourceInfo(unionType);
        return FieldDecl::Create(context,DC,StartLoc,IdLoc,fieldName, 
            unionType, TInfo, nullptr,false, ICIS_NoInit);
    }

    FieldDecl *createUnionArrayField(FieldDecl *FD){
        auto &context = FD->getASTContext();
        DeclContext *DC = FD->getParent();
        SourceLocation StartLoc = FD->getBeginLoc();
        SourceLocation IdLoc = FD->getEndLoc();
        IdentifierTable &identTable = context.Idents;
        IdentifierInfo *unionName = &identTable.get("union_" + FD->getNameAsString());
        CXXRecordDecl *unionDecl = CXXRecordDecl::Create(context,TagTypeKind::Union, DC, 
            StartLoc, IdLoc, unionName, nullptr, false);
        DC->addDecl(unionDecl);
        unionRecords.insert(unionDecl);

        unionDecl->startDefinition();
        FD->setDeclContext(unionDecl);
        unionDecl->addDecl(FD);
        auto size = context.getConstantArrayElementCount(
            context.getAsConstantArrayType(FD->getType()));
        QualType LLArray = context.getConstantArrayType(
            context.LongLongTy, llvm::APInt(64,size),nullptr,ArraySizeModifier::Normal,0);
        FieldDecl* LLArrayField = createField(FD, LLArray, "padding");
        LLArrayField->setDeclContext(unionDecl);
        unionDecl->addDecl(LLArrayField);
        unionDecl->completeDefinition();
        unionDecl->setCompleteDefinition();
        IdentifierInfo *unionArrayName = &identTable.get(FD->getNameAsString());
        QualType unionType = QualType(unionDecl->getTypeForDecl(),0);
        TypeSourceInfo *TInfo = context.CreateTypeSourceInfo(unionType);
        Expr *BW = nullptr;
        InClassInitStyle InitStyle = ICIS_NoInit;
        return FieldDecl::Create(context, DC, StartLoc, IdLoc, unionArrayName,
            unionType, TInfo, nullptr, false, InitStyle);
    }

    // Recursively set record field long to long long while modify pointer or pointer array field to union field
    // TODO or Not TODO: move set long to long long to ASTVisitor later
    // Opposition: 1. Modify it here doesn't need to judege which from udf param
    void revise(RecordDecl* RD){
        auto &context = RD->getASTContext();
        paramRecords.insert(RD);
        modifyPointerToUnion(RD);

        // QualType longIntType = context.getIntTypeForBitwidth(64,1);
        QualType longIntType = context.LongLongTy;

        for(auto it = RD->field_begin(); it != RD->field_end(); ++it){
            QualType field_qual_type = it->getType().getCanonicalType().getUnqualifiedType();
            const Type *field_type = it->getType().getTypePtr();
            if(field_type->isIntegerType() && field_qual_type.getAsString() == "long"){
                it->setType(longIntType);
                continue;
            }else if(field_type->isEnumeralType()){
                // 应该不需要处li?
                continue;
            }else if(field_type->isArrayType()){
                // don't use context.getBaseElementTpye. it represents the most inner type and ignore embeded array.
                // QualType element_qual_type = context.getBaseElementType(it->getType());
                QualType base_element_type = context.getBaseElementType(it->getType());
                if(base_element_type.getTypePtr()->isPointerType()){
                    continue;
                }
                if(base_element_type.getAsString() == "long"){
                    auto arrayType = context.getAsConstantArrayType(field_qual_type);
                    QualType element_qual_type = arrayType->getElementType();
                    
                    std::vector<unsigned> dimen(1, context.getConstantArrayElementCount(arrayType)); // the size for dimension, the index value for counts
                    // because context.getConstantArrayElementCount counts the total element, so only the last dimension is true
                    // and use the previous value to divide next value the get the actual counts.
                    
                    while(element_qual_type.getTypePtr()->isArrayType()){
                        arrayType = context.getAsConstantArrayType(element_qual_type);
                        element_qual_type = arrayType->getElementType();
                        dimen.push_back(context.getConstantArrayElementCount(arrayType));
                    }

                    for(int i=0; i<dimen.size()-1; ++i){
                        dimen[i] = dimen[i] / dimen[i+1];
                    }

                    QualType newArrayType = context.getConstantArrayType(longIntType, llvm::APInt(32, dimen[dimen.size()-1]), nullptr, ArraySizeModifier::Normal, 0);
                    for(int i=dimen.size()-2; i>=0; --i){
                        newArrayType = context.getConstantArrayType(newArrayType, llvm::APInt(32,dimen[i]), nullptr, ArraySizeModifier::Normal, 0);
                    }
                    // 设置数组类型
                    it->setType(newArrayType);
                    continue;
                }
                if(base_element_type->isRecordType()){
                    revise(base_element_type->getAsRecordDecl());
                }
            }else if(field_type->isRecordType()){   // contain struct, class and union
                revise(field_type->getAsRecordDecl());
            }
        }
    }

    bool findParamRecord(RecordDecl* RD){
        return paramRecords.count(RD);
    }

    bool findUnionRecord(RecordDecl* RD){
        return unionRecords.count(RD);
    }

    ValueDecl *getUnionField(FieldDecl *FD){
        return unionFieldMap[FD];
    }

};

}