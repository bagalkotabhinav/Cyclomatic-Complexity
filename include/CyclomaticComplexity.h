#ifndef CYCLOMATIC_COMPLEXITY_H
#define CYCLOMATIC_COMPLEXITY_H

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Analysis/CFG.h"
#include <string>
#include <map>

using namespace clang;

class CyclomaticComplexityVisitor : public RecursiveASTVisitor<CyclomaticComplexityVisitor> {
private:
    ASTContext *context;
    CompilerInstance &instance;
    std::map<std::string, int> ComplexityMap;

    bool isInHeader(Decl *decl);
    int calculateComplexity(const Stmt *stmt);
    void generateCFG(FunctionDecl *func);

public:
    explicit CyclomaticComplexityVisitor(ASTContext *context, CompilerInstance &instance);
    bool VisitFunctionDecl(FunctionDecl *func);
    void writeComplexityToFile(const std::string &filename);
};

class CyclomaticComplexityConsumer : public ASTConsumer {
private:
    CyclomaticComplexityVisitor visitor;

public:
    explicit CyclomaticComplexityConsumer(CompilerInstance &instance);
    virtual void HandleTranslationUnit(ASTContext &context) override;
};

class CyclomaticComplexityAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &instance, llvm::StringRef) override;
    bool ParseArgs(const CompilerInstance &, const std::vector<std::string> &) override;
};

#endif // CYCLOMATIC_COMPLEXITY_H