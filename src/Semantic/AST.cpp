#include "AST.hpp"

std::string toString(ASTType s) {
    switch(s) {
        case ASTType::LiteralNode: return "Literal";
        case ASTType::SimpleTypeNode: return "SimpleType";
        case ASTType::ArrayTypeNode: return "ArrayType";
        case ASTType::EnumTypeNode: return "EnumType";
        case ASTType::FieldTypeNode: return "FieldType";
        case ASTType::ConstDeclNode: return "ConstDecl";
        case ASTType::TypeDeclNode: return "TypeDecl";
        case ASTType::VarDeclNode: return "VarDecl";
        case ASTType::ParamDeclNode: return "ParamDecl";
        case ASTType::ProcDeclNode: return "ProcDecl";
        case ASTType::FuncDeclNode: return "FuncDecl";
        case ASTType::CallNode: return "Call";
        case ASTType::BinaryOpNode: return "BinaryOp";
        case ASTType::UnaryOpNode: return "UnaryOp";
        case ASTType::VarRefNode: return "VarRef";
        case ASTType::ArrayAccessNode: return "ArrayAccess";
        case ASTType::FieldAccessNode: return "FieldAccess";
        case ASTType::AssignNode: return "Assign";
        case ASTType::IfNode: return "If";
        case ASTType::WhileNode: return "While";
        case ASTType::ForNode: return "For";
        case ASTType::RepeatNode: return "Repeat";
        case ASTType::CaseNode: return "Case";
        case ASTType::CompoundNode: return "Compound";
        case ASTType::ProgramNode: return "Program";
        default: return "unknown";
    }
}