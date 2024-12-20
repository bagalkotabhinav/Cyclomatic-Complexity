#include "../include/CyclomaticComplexity.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>

CyclomaticComplexityVisitor::CyclomaticComplexityVisitor(ASTContext *context, CompilerInstance &instance)
    : context(context), instance(instance) {}

bool CyclomaticComplexityVisitor::isInHeader(Decl *decl) {
    auto loc = decl->getLocation();
    auto floc = context->getFullLoc(loc);
    if (floc.isInSystemHeader()) return true;
    auto entry = floc.getFileEntry()->getName();
    return entry.ends_with(".h") || entry.ends_with(".hpp");
}

int CyclomaticComplexityVisitor::calculateComplexity(const Stmt *stmt) {
    if (!stmt) return 0;
    int complexity = 1;  // Base complexity

    // Count decision points
    if (isa<IfStmt>(stmt) || isa<SwitchStmt>(stmt) || 
        isa<ForStmt>(stmt) || isa<WhileStmt>(stmt) || 
        isa<DoStmt>(stmt) || isa<ConditionalOperator>(stmt)) {
        complexity++;
    }

    // Process nested statements
    for (const Stmt *child : stmt->children()) {
        if (child) {
            complexity += calculateComplexity(child);
        }
    }
    return complexity;
}

void CyclomaticComplexityVisitor::generateCFG(FunctionDecl *func) {
    if (!func->hasBody()) return;

    std::unique_ptr<CFG> cfg = CFG::buildCFG(func, func->getBody(), context, CFG::BuildOptions());
    if (!cfg) return;

    std::string filename = func->getNameAsString() + "_cfg.dot";
    std::ofstream dot_file(filename);
    
    dot_file << "digraph CFG {\n";
    
    // Add nodes
    for (const auto *block : *cfg) {
        dot_file << "  Block" << block->getBlockID() << " [label=\"Block "
                << block->getBlockID() << "\\n";
        
        // Add block contents
        for (const auto &elem : *block) {
            std::optional<CFGStmt> stmt = elem.getAs<CFGStmt>();
            if (stmt.has_value()) {
                std::string stmtStr;
                llvm::raw_string_ostream stream(stmtStr);
                stmt.value().getStmt()->printPretty(stream, nullptr, PrintingPolicy(instance.getLangOpts()));
                dot_file << stream.str() << "\\n";
            }
        }
        dot_file << "\"]\n";

        // Add edges
        for (auto I = block->succ_begin(), E = block->succ_end(); I != E; ++I) {
            if (const CFGBlock *succ = *I) {
                dot_file << "  Block" << block->getBlockID() 
                        << " -> Block" << succ->getBlockID() << "\n";
            }
        }
    }
    dot_file << "}\n";
}

bool CyclomaticComplexityVisitor::VisitFunctionDecl(FunctionDecl *func) {
    if (!func->hasBody() || isInHeader(func)) return true;

    int complexity = calculateComplexity(func->getBody());
    ComplexityMap[func->getNameAsString()] = complexity;
    llvm::outs() << "Function: " << func->getNameAsString() 
                << ", Complexity: " << complexity << "\n";
    generateCFG(func);
    return true;
}

void CyclomaticComplexityVisitor::writeComplexityToFile(const std::string &filename) {
    std::ofstream outFile(filename);
    for (const auto &entry : ComplexityMap) {
        outFile << "Function: " << entry.first 
                << ", Cyclomatic Complexity: " << entry.second << "\n";
    }
}

CyclomaticComplexityConsumer::CyclomaticComplexityConsumer(CompilerInstance &instance)
    : visitor(&instance.getASTContext(), instance) {}

void CyclomaticComplexityConsumer::HandleTranslationUnit(ASTContext &context) {
    visitor.TraverseDecl(context.getTranslationUnitDecl());
    visitor.writeComplexityToFile("complexity_results.txt");
}

std::unique_ptr<ASTConsumer> CyclomaticComplexityAction::CreateASTConsumer(
    CompilerInstance &instance, llvm::StringRef) {
    return std::make_unique<CyclomaticComplexityConsumer>(instance);
}

bool CyclomaticComplexityAction::ParseArgs(const CompilerInstance &, 
                                         const std::vector<std::string> &) {
    return true;
}

static FrontendPluginRegistry::Add<CyclomaticComplexityAction> 
X("cyclomatic-complexity", "Calculate cyclomatic complexity and generate CFG");