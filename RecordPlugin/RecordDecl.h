#pragma once
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/CodeGen/ModuleBuilder.h"

#include "CodeGen/CodeGenTypes.h"
#include "CodeGen/CodeGenModule.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

#include <string>
#include <vector>

// #include "ComputeOffset.h"

namespace clang{

struct RecordDeclAction : public PluginASTAction {
    RecordDeclAction(){};

    std::unique_ptr<ASTConsumer>
        CreateASTConsumer(CompilerInstance &CI, StringRef InFile) override;

    ActionType getActionType() override {return CmdlineAfterMainAction;}

    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &Args) override;

    // void init(CompilerInstance &CI);

private:
    std::unique_ptr<ast_matchers::MatchFinder> ASTFinder;
    std::unique_ptr<ast_matchers::MatchFinder::MatchCallback> UDFCb;
    llvm::StringRef udf;
    // llvm::LLVMContext VMCxt;    // must put before TheModule, because TheModule construct with VMCxt
    // llvm::Module TheModule;
    // std::unique_ptr<CodeGen::CodeGenModule> CGM;
    // std::unique_ptr<CodeGen::CodeGenTypes> CGT;
    // std::unique_ptr<ComputeOffset> CO;
};
}