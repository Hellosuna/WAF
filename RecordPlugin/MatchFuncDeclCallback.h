#pragma once
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Serialization/PCHContainerOperations.h"
#include "clang/Serialization/ASTWriter.h"
#include "clang/CodeGen/BackendUtil.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/AST/RecursiveASTVisitor.h"

// my file
#include "ASTVisitor.h"
#include "RecordRevise.h"

#include <string>
namespace clang{
using namespace ast_matchers;

struct MatchFunctionDeclCallback : public MatchFinder::MatchCallback {
private:
    CompilerInstance &ci;
    llvm::StringRef FileName;
public:
    MatchFunctionDeclCallback(CompilerInstance &CI, llvm::StringRef InFile) 
        : ci(CI), FileName(InFile){}
    
    void run(const MatchFinder::MatchResult &Result) override {
        const auto& Nodes = Result.Nodes;

        auto* Function = Nodes.getNodeAs<FunctionDecl>("function");
        if(!Function)
            return;

        RecordRevise recordRevise;
        for(auto it = Function->param_begin(); it != Function->param_end(); ++it){
            auto *type = (*it)->getType().getCanonicalType().getTypePtr();
            if(type->isLValueReferenceType()){
                type = ((LValueReferenceType*)type)->getPointeeType().getCanonicalType().getTypePtr();
            }
            if(type->isRecordType()){   
                recordRevise.revise(type->getAsRecordDecl());
                continue;
            }
        }
        MyASTVisitor visitor(ci, recordRevise);
        auto &ctx = Function->getASTContext();
        visitor.modifyPointMemberExpr(ctx);
        visitor.modifyLongType(ctx);
        ctx.getTranslationUnitDecl()->dump();
    }

    void onStartOfTranslationUnit(){

    }

    void onEndOfTranslationUnit() {
        // TODO: directly join CodeGen ASTConsumer, traverse the ast to handleTopDecl
        // has no meanings, because CodeGenAction's CreateASTConsumer is a protected method
        // and we should move all the BackendConsumer to our plugin which is complex!
        // Besides, codegen action is closely related to parse,preprocessor,lex.
        // And clang doesn't support load ast from memory buffer only file
        // try to use CI.Executetion until it is supported

        // In comparison, generate ast is more convenient
        std::string Sysroot = ci.getHeaderSearchOpts().Sysroot;
        if (ci.getFrontendOpts().RelocatablePCH && Sysroot.empty()) {
            printf("sysroot false\n");
        }
        if (!ci.getFrontendOpts().RelocatablePCH)
            Sysroot.clear();
        std::string OutputFile = ci.getFrontendOpts().OutputFile;
        std::unique_ptr<raw_pwrite_stream> OS = 
            ci.createDefaultOutputFile(true,FileName,"",false);
        if(!OS){
            llvm::outs() << "CI createDefaultOutputFile failed\n";
            return;
        }
        const auto &FrontendOpts = ci.getFrontendOpts();
        auto Buffer = std::make_shared<PCHBuffer>();
        auto PCHGenConsumer = std::make_unique<PCHGenerator>(
            ci.getPreprocessor(), ci.getModuleCache(), OutputFile, Sysroot, Buffer,
            FrontendOpts.ModuleFileExtensions,
            ci.getPreprocessorOpts().AllowPCHWithCompilerErrors,
            FrontendOpts.IncludeTimestamps, FrontendOpts.BuildingImplicitModule,
            +ci.getLangOpts().CacheGeneratedPCH);
        PCHGenConsumer->InitializeSema(ci.getSema());
        PCHGenConsumer->HandleTranslationUnit(ci.getASTContext());
        auto PCHWriter = ci.getPCHContainerWriter().CreatePCHContainerGenerator(
            ci, std::string(FileName), OutputFile, std::move(OS), Buffer);
        PCHWriter->HandleTranslationUnit(ci.getASTContext());
    }

};
}