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

int SemanticAnalyzer::visitExpr(std::shared_ptr<ExprNode> node) {
    if (!node) return T_NONE;
    int t = T_NONE;
    switch (node->getASTType()) {
        case ASTType::LiteralNode:     t = visitLiteral    (std::dynamic_pointer_cast<LiteralNode>    (node)); break;
        case ASTType::VarRefNode:      t = visitVarRef     (std::dynamic_pointer_cast<VarRefNode>     (node)); break;
        case ASTType::BinaryOpNode:    t = visitBinaryOp   (std::dynamic_pointer_cast<BinaryOpNode>   (node)); break;
        case ASTType::UnaryOpNode:     t = visitUnaryOp    (std::dynamic_pointer_cast<UnaryOpNode>    (node)); break;
        case ASTType::CallNode:        t = visitCall       (std::dynamic_pointer_cast<CallNode>       (node)); break;
        case ASTType::ArrayAccessNode: t = visitArrayAccess(std::dynamic_pointer_cast<ArrayAccessNode>(node)); break;
        case ASTType::FieldAccessNode: t = visitFieldAccess(std::dynamic_pointer_cast<FieldAccessNode>(node)); break;
        default: break;
    }
    return t;
}

int SemanticAnalyzer::visitType(std::shared_ptr<TypeNode> node) {
    if (!node) return T_NONE;
    switch (node->getKind()) {
        case TypeNode::Kind::Simple:     return visitSimpleType(std::dynamic_pointer_cast<SimpleTypeNode>(node));
        case TypeNode::Kind::Array:      return visitArrayType (std::dynamic_pointer_cast<ArrayTypeNode> (node));
        case TypeNode::Kind::Enumerated: return visitEnumType  (std::dynamic_pointer_cast<EnumTypeNode>  (node));
        case TypeNode::Kind::Range:      return visitRangeType (std::dynamic_pointer_cast<RangeTypeNode> (node));
        case TypeNode::Kind::Record:     return visitFieldType (std::dynamic_pointer_cast<FieldTypeNode> (node));
        default: return T_NONE;
    }
}

