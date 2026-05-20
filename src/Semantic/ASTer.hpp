#pragma once

#include "AST.hpp"
#include "../Parser/ParseTree.hpp"

class ASTer {
public:
    std::shared_ptr<ASTNode> buildAST(TreeNode root) const;
    std::shared_ptr<ASTNode> buildSimpleTypeNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildArrayTypeNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildEnumTypeNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildFieldTypeNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildConstDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildTypeDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildVarDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildParamDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildProcDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildFuncDeclNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildLiteralNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildCallNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildBinaryOpNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildUnaryOpNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildVarRefNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildArrayAccessNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildFieldAccessNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildAssignNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildIfNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildWhileNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildForNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildRepeatNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildCaseNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildCompoundNode(TreeNode root) const;
    std::shared_ptr<ASTNode> buildProgramNode(TreeNode root) const;
};