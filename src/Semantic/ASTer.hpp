#pragma once

#include "AST.hpp"
#include "../Parser/ParseTree.hpp"

class ASTer {
public:
    std::shared_ptr<ASTNode> buildAST(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<SimpleTypeNode> buildSimpleTypeNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ArrayTypeNode> buildArrayTypeNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<EnumTypeNode> buildEnumTypeNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<FieldTypeNode> buildFieldTypeNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<DeclNode> buildDeclNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<LiteralNode> buildLiteralNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CallNode> buildCallNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<BinaryOpNode> buildBinaryOpNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<UnaryOpNode> buildUnaryOpNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<VarRefNode> buildVarRefNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ArrayAccessNode> buildArrayAccessNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<FieldAccessNode> buildFieldAccessNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<AssignNode> buildAssignNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<IfNode> buildIfNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<WhileNode> buildWhileNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ForNode> buildForNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<RepeatNode> buildRepeatNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CaseNode> buildCaseNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CompoundNode> buildCompoundNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ProgramNode> buildProgramNode(std::shared_ptr<TreeNode> root) const;
};