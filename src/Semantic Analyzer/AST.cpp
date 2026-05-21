#include "AST.hpp"
#include "../Parser/ParseTree.hpp"

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
        case ASTType::CallStmtNode: return "CallStmt";
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

std::shared_ptr<LiteralNode> constantToLiteral(std::shared_ptr<TreeNode> node) {
    std::vector<std::shared_ptr<TreeNode>> children = node->getChildren();
    
    std::string sign = "";
    std::shared_ptr<TreeNode> valueNode;

    if (children[0]->getNodeType() == Symbol::plus || children[0]->getNodeType() == Symbol::minus) {
        sign = children[0]->getNodeType() == Symbol::plus ? "+" : "-";
        valueNode = children[1];
    } else {
        valueNode = children[0];
    }

    Symbol sym = valueNode->getNodeType();
    std::string val = valueNode->getValue();

    if (sym == Symbol::charcon) {
        return std::make_shared<LiteralNode>(LiteralKind::Char, val);
    } else if (sym == Symbol::string) {
        return std::make_shared<LiteralNode>(LiteralKind::String, val);
    } else if (sym == Symbol::intcon) {
        return std::make_shared<LiteralNode>(LiteralKind::Int, sign + val);
    } else if (sym == Symbol::realcon) {
        return std::make_shared<LiteralNode>(LiteralKind::Real, sign + val);
    } else if (sym == Symbol::ident) {
        return std::make_shared<LiteralNode>(LiteralKind::Int, sign + val);
    }

    throw std::runtime_error("Unknown constant type");
}

std::vector<std::string> enumToStrings(std::shared_ptr<TreeNode> node){
    std::vector<std::string> values;
    for (auto& child : node->getChildren()) {
        if (child->getNodeType() == Symbol::ident) {
            values.push_back(child->getValue());
        }
    }
    return values;
}

std::vector<std::string> identifierListToStrings(std::shared_ptr<TreeNode> node) {
    std::vector<std::string> names;
    for (auto& child : node->getChildren()) {
        if (child->getNodeType() == Symbol::ident) {
            names.push_back(child->getValue());
        }
    }
    return names;
}