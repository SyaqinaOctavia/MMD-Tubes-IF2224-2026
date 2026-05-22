#include "SemanticAnalyzer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

void SemanticAnalyzer::visit(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    switch (node->getASTType()) {
        case ASTType::ProgramNode:   visitProgram  (std::dynamic_pointer_cast<ProgramNode> (node)); break;
        case ASTType::ConstDeclNode: visitConstDecl(std::dynamic_pointer_cast<ConstDeclNode>(node)); break;
        case ASTType::TypeDeclNode:  visitTypeDecl (std::dynamic_pointer_cast<TypeDeclNode> (node)); break;
        case ASTType::VarDeclNode:   visitVarDecl  (std::dynamic_pointer_cast<VarDeclNode>  (node)); break;
        case ASTType::ParamDeclNode: visitParamDecl(std::dynamic_pointer_cast<ParamDeclNode>(node)); break;
        case ASTType::ProcDeclNode:  visitProcDecl (std::dynamic_pointer_cast<ProcDeclNode> (node)); break;
        case ASTType::FuncDeclNode:  visitFuncDecl (std::dynamic_pointer_cast<FuncDeclNode> (node)); break;
        case ASTType::AssignNode:    visitAssign   (std::dynamic_pointer_cast<AssignNode>   (node)); break;
        case ASTType::IfNode:        visitIf       (std::dynamic_pointer_cast<IfNode>       (node)); break;
        case ASTType::WhileNode:     visitWhile    (std::dynamic_pointer_cast<WhileNode>    (node)); break;
        case ASTType::ForNode:       visitFor      (std::dynamic_pointer_cast<ForNode>      (node)); break;
        case ASTType::RepeatNode:    visitRepeat   (std::dynamic_pointer_cast<RepeatNode>   (node)); break;
        case ASTType::CaseNode:      visitCase     (std::dynamic_pointer_cast<CaseNode>     (node)); break;
        case ASTType::CompoundNode:  visitCompound (std::dynamic_pointer_cast<CompoundNode> (node)); break;
        case ASTType::CallStmtNode:  visitCallStmt (std::dynamic_pointer_cast<CallStmtNode> (node)); break;
        default: break;
    }
}
