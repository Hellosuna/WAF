#include "RecordDecl.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/FrontendTool/Utils.h"
#include "clang/AST/RecordLayout.h"
#include "clang/CodeGen/CodeGenAction.h"


#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/VirtualFileSystem.h"

#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include "MatchFuncDeclCallback.h"

#include <vector>

using namespace clang;
using namespace ast_matchers;

bool RecordDeclAction::ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &Args){
    // TODO: process cmdline args, such as udf name
    udf = "udf";
    return true;
}

// match functionDecl whicn has any paramVarDecl, which is RecordDecl, can specified name
static DeclarationMatcher buildFunctionDeclMatcher(StringRef name){
    return functionDecl(
        isExpansionInMainFile()
        , hasName(name)
    ).bind("function");
}

// RecordDeclAction::RecordDeclAction():TheModule("module", VMCxt){}

// void RecordDeclAction::init(CompilerInstance &CI){
//     llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS(&CI.getVirtualFileSystem());
//     CGM = std::make_unique<CodeGen::CodeGenModule>(
//         CI.getASTContext(), FS,
//         CI.getHeaderSearchOpts(),
//         CI.getPreprocessorOpts(),
//         CI.getCodeGenOpts(),
//         TheModule,
//         CI.getDiagnostics()
//     );
//     CGT = std::make_unique<CodeGen::CodeGenTypes>(*CGM);
//     CO = std::make_unique<ComputeOffset>(VMCxt, *CGT);
// }

std::unique_ptr<ASTConsumer> RecordDeclAction::CreateASTConsumer(
    CompilerInstance &CI, StringRef InFile){
    assert(CI.hasASTContext() && "No ASTContext??");
    // init(CI);
    ASTFinder = std::make_unique<MatchFinder>();
    UDFCb = std::make_unique<MatchFunctionDeclCallback>(CI, InFile);
    ASTFinder->addMatcher(
        traverse(TK_IgnoreUnlessSpelledInSource,buildFunctionDeclMatcher(udf)),
        UDFCb.get());
    return std::move(ASTFinder->newASTConsumer());
}
static FrontendPluginRegistry::Add<RecordDeclAction>
    X("record", "find function param with record");


