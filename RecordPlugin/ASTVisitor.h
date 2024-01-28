#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ASTImporter.h"
// #include "clang/AST/Decl.h"

#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "RecordRevise.h"

namespace clang{
using namespace ast_matchers;
class MyASTVisitor{

public:

    // class member initial must be put after constructor ":"
    MyASTVisitor(CompilerInstance &CI, RecordRevise &recordRevise)
        :ci(CI)
        ,ctx(ci.getASTContext())
        ,recordRevise(recordRevise)
        ,memberVisitor(ctx, *this)
        ,longVisitor(ctx, *this)
        {}

    void modifyRightMemberExpr(FunctionDecl * funcDecl){
        memberVisitor.TraverseDecl(funcDecl);
        // TODO:: every time after traversed clear previous value decl.
        // Because this will add new value decl and may traverse funcion decl again
        // So this need a while loop untial value decl num to be zero.
    }

    void modifyPointMemberExpr(FunctionDecl *funcDecl){
        memberVisitor.TraverseDecl(funcDecl);
    }

    void modifyPointMemberExpr(ASTContext &context){
        memberVisitor.TraverseAST(context);
    }

    void modifyLongType(FunctionDecl *funcDecl){
        longVisitor.TraverseDecl(funcDecl);
    }

    void modifyLongType(ASTContext &context){
        llvm::outs() << "Begin modifying long type\n";
        longVisitor.TraverseAST(context);
    }

    void insertValueDecl(ValueDecl* valueDecl, std::string type){
        valueDecls.insert(std::make_pair(valueDecl, type));
    }

    bool findValueDecl(ValueDecl *valudeDecl){
        if(valueDecls.count(valudeDecl))
            return true;
        return false;
    }

    bool findUnionRecord(RecordDecl* RD){
        return recordRevise.findUnionRecord(RD);
    }

    void insertExcludedUO(UnaryOperator *uo){
        excludedUOs.insert(uo);
    }

    bool findExcludedUO(UnaryOperator *uo){
        if(excludedUOs.count(uo))
            return true;
        return false;
    }

    bool findParamRecord(RecordDecl* RD){
        return recordRevise.findParamRecord(RD);
    }

    ValueDecl *getUnionField(FieldDecl *FD){
        return recordRevise.getUnionField(FD);
    }

    SourceManager &getSourceManager(){
        return ci.getSourceManager();
    }

private:
    
    class MemberExprVisitor : public RecursiveASTVisitor<MemberExprVisitor>{
    private:
        ASTContext &ctx;
        MyASTVisitor &myVisitor;
        std::unordered_set<MemberExpr*> pointerArrayMember;
        // record member array sub info;
        struct ArrayDimen {
            std::vector<uint64_t> dimens;
            int index = 1;
        };
        // ImpilicitCastExpr, ArraySubscriptExpr and so on  map to MemberExpr
        // These Exprs are all MemberExpr parent node
        std::unordered_map<Expr*, MemberExpr*> ExprMapMember;
        // MemberExpr map to top layer which may be: ImpliciCastExpr, ArraySubscriptExpr
        std::unordered_map<MemberExpr*, struct ArrayDimen> memberArrayDimen;

    public:
        MemberExprVisitor(ASTContext &context, MyASTVisitor &myASTVisitor)
            :ctx(context), myVisitor(myASTVisitor){}
        
        bool shouldTraversePostOrder(){return true;}    // default preorder
        
        bool shouldVisitTemplateInstantiations(){return true;}
        
        bool VisitMemberExpr(MemberExpr *memberExpr){
            ValueDecl *memberDecl = memberExpr->getMemberDecl();
            // llvm::outs() << "member name: " << memberDecl->getNameAsString() 
            //     << " member qual type: " << memberDecl->getType().getAsString() 
            //     << " member type: " << memberDecl->getType().getTypePtr()->getTypeClassName()
            //     << "\n";
            FieldDecl *fieldDecl = dyn_cast<FieldDecl>(memberDecl);
            if(!fieldDecl)
                return true;

            const Type* memberType = fieldDecl->getType().getCanonicalType().getTypePtr();
            if(memberType->isPointerType()){
                // the origin fielddecl is now new union field
                auto *recordDecl = (RecordDecl*) fieldDecl->getParent()->getParent();
                if(!myVisitor.findParamRecord(recordDecl)){
                    return true;
                }
                // mark MemberExpr
                ValueDecl* memberDecl = myVisitor.getUnionField(fieldDecl);
                MemberExpr* unionMember = createMemberExpr(memberExpr->getBase(), memberExpr, memberDecl);
                memberExpr->setBase(unionMember);
            }else if(memberType->isConstantArrayType()){
                if(!memberType->getBaseElementTypeUnsafe()->isPointerType())
                    return true;
                auto *recordDecl = (RecordDecl*) fieldDecl->getParent();
                if(!myVisitor.findParamRecord(recordDecl))
                    return true;
                pointerArrayMember.insert(memberExpr);
                ValueDecl* memberDecl = myVisitor.getUnionField(fieldDecl);
                MemberExpr* unionMember = createMemberExpr(memberExpr->getBase(), memberExpr, memberDecl);
                memberExpr->setBase(unionMember);
                // union is not a pointer but origin MemberExpr may be arrow
                memberExpr->setArrow(false);
                std::vector<uint64_t> arrayDimen;
                const ConstantArrayType* array = myVisitor.ctx.getAsConstantArrayType(fieldDecl->getType());
                while(array){
                    arrayDimen.push_back(array->getSize().getZExtValue());
                    array = myVisitor.ctx.getAsConstantArrayType(array->getElementType());
                }
                struct ArrayDimen dimen = {std::move(arrayDimen)};
                memberArrayDimen.insert(std::make_pair(memberExpr, dimen));
            }
            return true;
        }

        // 1. MemberExpr parent node may be ImplicitCastExpr, UnaryOperator, ArraySubscriptExpr.
        // 2. SubscriptExpr parent may be ImplicitCastExpr, UnaryOperator where to modify SubscriptExpr.
        bool VisitImplicitCastExpr(ImplicitCastExpr *implicitCast){
            Expr *subExpr = implicitCast->getSubExpr();
            if(auto sub = modifyArraySubscriptExpr(implicitCast, subExpr)){
                // implicitCast->setCastKind(CK_NoOp);
                implicitCast->setSubExpr(sub);
            }
            return true;
        }

        bool VisitArraySubscriptExpr(ArraySubscriptExpr* arraySub){
            auto *lhs = arraySub->getLHS();
            if(ExprMapMember.count(lhs))
                ExprMapMember.insert(std::make_pair(arraySub, ExprMapMember.at(lhs)));
            return true;
        }

        bool VisitUnaryOperator(UnaryOperator* uo){
            Expr *subExpr = uo->getSubExpr();  
            if(auto *sub = modifyArraySubscriptExpr(uo, subExpr))
                uo->setSubExpr(sub);

            return true;
        }

        Expr* modifyArraySubscriptExpr(Expr* expr, Expr* subExpr){
            if(auto *memberExpr = dyn_cast<MemberExpr>(subExpr)){
                if(pointerArrayMember.count(memberExpr))
                    ExprMapMember.insert(std::make_pair(expr,memberExpr));
                return nullptr;
            }

            if(ExprMapMember.count(subExpr))
                ExprMapMember.insert(std::make_pair(expr, ExprMapMember[subExpr]));
            else
                return nullptr;

            if(auto *arraySub = dyn_cast<ArraySubscriptExpr>(subExpr)){
                auto *type = arraySub->getType().getTypePtr();
                if(type->isConstantArrayType()){
                    if(!type->getBaseElementTypeUnsafe()->isPointerType())
                        return nullptr;
                }else if(!type->isPointerType())
                    return nullptr;
                // only if the ArraySubscriptExpr return pointer type, it need to compute offset
                auto *addrOf = createUO(arraySub, UO_AddrOf);
                auto *intCast = createCast(addrOf, ctx.IntTy, CK_PointerToIntegral);

                struct ArrayDimen &AD = memberArrayDimen.at(ExprMapMember[subExpr]);
                Expr *subscript = arraySub->getRHS();
                uint64_t size = 4;
                // every time compute, index++
                for(int i = AD.index++; i<AD.dimens.size(); ++i){
                    size *= AD.dimens[i];
                }
                auto *intLiteral = IntegerLiteral::Create(
                    ctx,llvm::APInt(64,size),ctx.IntTy,intCast->getExprLoc());
                auto *offset = createBinaryOperator(subscript, intLiteral, BO_Mul);
                auto *addBo = createBinaryOperator(intCast, offset, BO_Add);
                auto *paren = new (ctx) ParenExpr(addBo->getBeginLoc(), addBo->getEndLoc(),addBo);

                QualType pointerType = type->isPointerType()?arraySub->getType():
                    ctx.getPointerType(ctx.getAsConstantArrayType(arraySub->getType())->getElementType());
                auto *pointCast = createCast(paren, addrOf->getType(), CK_IntegralToPointer);
                auto *pointCastParen = createParen(pointCast);
                auto *deref = createUO(pointCastParen, UO_Deref);
                return deref;
            }
            return nullptr;
            
        }

       
        MemberExpr* createMemberExpr(Expr* Base, MemberExpr *memberExpr, ValueDecl *memberDecl){
            bool isArrow = memberExpr->isArrow();
            SourceLocation OperatorLoc = memberExpr->getBeginLoc();
            NestedNameSpecifierLoc QualifierLoc = memberExpr->getQualifierLoc();
            SourceLocation TemplateKWLoc = memberExpr->getTemplateKeywordLoc();
            DeclAccessPair FoundDecl = memberExpr->getFoundDecl();
            DeclarationNameInfo MemberNameInfo = memberExpr->getMemberNameInfo();
            TemplateArgumentListInfo TemplateArgs(
                memberExpr->getLAngleLoc(),memberExpr->getRAngleLoc()
                );
            for(auto arg : memberExpr->template_arguments()){
                TemplateArgs.addArgument(arg);
            }
            QualType T = memberDecl->getType();
            ExprValueKind VK = memberExpr->getValueKind();
            ExprObjectKind OK = memberExpr->getObjectKind();
            NonOdrUseReason NOUR = memberExpr->isNonOdrUse();
            return MemberExpr::Create(myVisitor.ctx,Base
                ,isArrow,OperatorLoc,QualifierLoc,TemplateKWLoc,memberDecl,
                FoundDecl,MemberNameInfo,&TemplateArgs,T,VK,OK,NOUR);
        }

        // 1. & to get address of member
        // 2. * to deref member address
        UnaryOperator *createUO(Expr *expr, UnaryOperator::Opcode opc){
            QualType type = expr->getType();
            if(opc == UO_AddrOf){   // we need respond member pointer type
                type = ctx.getPointerType(type);
            }else if(opc == UO_Deref){
                type = type.getTypePtr()->getPointeeType();
            }
            ExprValueKind VK = expr->getValueKind();
            ExprObjectKind OK = expr->getObjectKind();
            SourceLocation l = expr->getExprLoc();
            FPOptionsOverride FPO;
            return UnaryOperator::Create(ctx,expr,opc,type,VK,OK,l,false,FPO);
        }

        BinaryOperator *createBinaryOperator(Expr *lhs, Expr *rhs, BinaryOperator::Opcode opc){
            ExprValueKind VK = lhs->getValueKind();
            ExprObjectKind OK = lhs->getObjectKind();
            SourceLocation opLoc = lhs->getBeginLoc();
            QualType ResTy = lhs->getType();
            FPOptionsOverride FPO;
            return BinaryOperator::Create(ctx,lhs,rhs,opc,ResTy,VK,OK,opLoc,FPO);
        }

        CStyleCastExpr *createCast(Expr *expr, QualType type, CastKind K){
            ExprValueKind VK = expr->getValueKind();
            // CXXCastPath 是一个包含多个 CXXBaseSpecifier 元素的数组
            // 包含了有关基类的信息，例如基类的类型、访问权限等
            // 主要用于 C++ 风格的类型转换（cast）中，其中可能涉及到类层次结构中的多个继承路径。
            // 这种信息对于静态转型（static_cast）等操作是必要的，因为它确保了在转型时正确地选择了路径。
            const CXXCastPath *BasePath = nullptr; 
            FPOptionsOverride FPO;
            TypeSourceInfo *WrittenTy = ctx.getTrivialTypeSourceInfo(type, expr->getBeginLoc());
            SourceLocation L = expr->getBeginLoc();
            SourceLocation R = expr->getEndLoc();
            return CStyleCastExpr::Create(ctx,type,VK,K,expr,BasePath,FPO,WrittenTy,L,R);
        }

        ParenExpr *createParen(Expr *val){
            return new (ctx) ParenExpr(val->getBeginLoc(),val->getEndLoc(),val);
        }
  
    };

    class LongVisitor : public RecursiveASTVisitor<LongVisitor>{
    private:
        ASTContext &ctx;
        MyASTVisitor &myVisitor;
    public:

        bool shouldVisitTemplateInstantiations(){return true;}

        LongVisitor(ASTContext &context, MyASTVisitor &myASTVisitor)
            :ctx(context), myVisitor(myASTVisitor){}

        bool VisitExpr(Expr *expr){
            if(!ctx.getSourceManager().isWrittenInMainFile(expr->getExprLoc()))
                return true;
            QualType T = getLLTy(expr->getType());
            if(T != ctx.NullPtrTy)
                expr->setType(T);
            return true;
        }

        bool VisitVarDecl(VarDecl *varDecl){
            if(!ctx.getSourceManager().isWrittenInMainFile(varDecl->getLocation()))
                return true;
            QualType T = getLLTy(varDecl->getType());
            if(T != ctx.NullPtrTy)
                varDecl->setType(T);
            return true;
        }

        bool VisitDeclStmt(DeclStmt *declStmt){
            return true;
        }

        QualType getLLTy(QualType QType){
            if(QType.isNull()) // QualType may be null: void type!
                return ctx.NullPtrTy;
            unsigned Quals = QType.getCVRQualifiers();
            QualType canType = QType.getCanonicalType().getUnqualifiedType();
            QualType newType;
            if(canType.getAsString() == "long"){
                newType = ctx.LongLongTy;
            }else if(canType.getAsString() == "unsigned long"){
                newType = ctx.UnsignedLongLongTy;
            }else if(canType.getAsString() == "long *"){
                newType = ctx.getPointerType(ctx.LongLongTy);
            }else if(canType.getAsString() == "unsigned long *"){
                newType = ctx.getPointerType(ctx.UnsignedLongLongTy);
            }else{
                return ctx.NullPtrTy;
            }
            newType.setLocalFastQualifiers(Quals);
            return newType;
        }
    };

    // TODO:: memcopy point array
    class CopyVisitor : public RecursiveASTVisitor<CopyVisitor>{
    public:
        bool VisitVarDecl(VarDecl *varDecl){
            varDecl->getBody();
            return true;
        }
    };

private:
    CompilerInstance &ci;
    ASTContext &ctx;
    MemberExprVisitor memberVisitor;
    LongVisitor longVisitor;

    RecordRevise &recordRevise;

    std::unordered_map<ValueDecl*, std::string> valueDecls;
    std::unordered_set<UnaryOperator*> excludedUOs;
};

}