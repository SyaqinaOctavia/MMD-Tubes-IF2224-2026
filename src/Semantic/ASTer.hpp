#pragma once

#include "AST.hpp"
#include "../Parser/ParseTree.hpp"

class ASTer {
public:
    ASTer(){}
    std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>> fieldPartToField(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildSimpleType(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildArrayType(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildRangeType(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildEnumType(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildRecordType(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<TypeNode> buildArrayIndex(std::shared_ptr<TreeNode> node) const;
    std::shared_ptr<ASTNode> buildAST(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<TypeNode> buildTypeNode(std::shared_ptr<TreeNode> root) const;
    std::vector<std::shared_ptr<DeclNode>> buildDeclNodes(std::shared_ptr<TreeNode> root) const;
    std::vector<std::shared_ptr<ConstDeclNode>> buildConstDeclNode(std::shared_ptr<TreeNode> root) const;
    std::vector<std::shared_ptr<TypeDeclNode>> buildTypeDeclNode(std::shared_ptr<TreeNode> root) const;
    std::vector<std::shared_ptr<VarDeclNode>> buildVarDeclNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ParamDeclNode> buildParamDeclNode(std::shared_ptr<TreeNode> root) const;
    std::vector<std::shared_ptr<ParamDeclNode>> buildFormalParams(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ProcDeclNode> buildProcDeclNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<FuncDeclNode> buildFuncDeclNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<LiteralNode> buildLiteralNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CallNode> buildCallNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<BinaryOpNode> buildBinaryOpNode(std::shared_ptr<ExprNode> lhs, std::shared_ptr<TreeNode> opNode, std::shared_ptr<ExprNode> rhs) const;
    std::shared_ptr<UnaryOpNode> buildUnaryOpNode(std::shared_ptr<TreeNode> opNode, std::shared_ptr<ExprNode> operand) const;
    std::shared_ptr<VarRefNode> buildVarRefNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ArrayAccessNode> buildArrayAccessNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<FieldAccessNode> buildFieldAccessNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<AssignNode> buildAssignNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<IfNode> buildIfNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ExprNode> buildExprNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<StmtNode> buildStmtNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<WhileNode> buildWhileNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ForNode> buildForNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<RepeatNode> buildRepeatNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CaseNode> buildCaseNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CompoundNode> buildCompoundNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<ProgramNode> buildProgramNode(std::shared_ptr<TreeNode> root) const;
    std::shared_ptr<CompoundNode> buildStatementList(std::shared_ptr<TreeNode> root) const;
};