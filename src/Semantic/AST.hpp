#pragma once

#include <vector>
#include <memory>
#include "../Symbol.hpp"

enum class ASTType {
    LiteralNode,
    TypeNode,
    DeclNode,
    CallNode,
    BinaryOpNode, 
    UnaryOpNode,
    VarRefNode, 
    ArrayAccessNode, 
    FieldAccessNode,
    IfNode, 
    WhileNode, 
    ForNode, 
    RepeatNode, 
    CaseNode, 
    AssignNode, 
    CompoundNode,
    ProgramNode
};

class ASTNode {
    private:
        ASTType NodeType;
        std::vector<std::shared_ptr<ASTNode>> Children;
};