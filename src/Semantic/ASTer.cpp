#include "ASTer.hpp"
#include "../Symbol.hpp"
#include <string>

std::shared_ptr<ASTNode> ASTer::buildAST(std::shared_ptr<TreeNode> root) const {
    return buildProgramNode(root);
}
std::shared_ptr<SimpleTypeNode> ASTer::buildSimpleTypeNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ArrayTypeNode> ASTer::buildArrayTypeNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<EnumTypeNode> ASTer::buildEnumTypeNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<FieldTypeNode> ASTer::buildFieldTypeNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ConstDeclNode> ASTer::buildConstDeclNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<DeclNode> ASTer::buildDeclNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<LiteralNode> ASTer::buildLiteralNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<CallNode> ASTer::buildCallNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<BinaryOpNode> ASTer::buildBinaryOpNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<UnaryOpNode> ASTer::buildUnaryOpNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<VarRefNode> ASTer::buildVarRefNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ArrayAccessNode> ASTer::buildArrayAccessNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<FieldAccessNode> ASTer::buildFieldAccessNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<AssignNode> ASTer::buildAssignNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<IfNode> ASTer::buildIfNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<WhileNode> ASTer::buildWhileNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ForNode> ASTer::buildForNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<RepeatNode> ASTer::buildRepeatNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<CaseNode> ASTer::buildCaseNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<CompoundNode> ASTer::buildCompoundNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ProgramNode> ASTer::buildProgramNode(std::shared_ptr<TreeNode> root) const {
    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();
    std::string name;
    std::shared_ptr<DeclNode> decl_node;
    std::shared_ptr<CompoundNode> comp_node;
    if(children[0]->getNodeType() == Symbol::PROGRAM_HEADER){
        std::vector<std::shared_ptr<TreeNode>> grandchildren = children[0]->getChildren();
        name = grandchildren[1]->getValue();
    } else return nullptr;
    if(children[1]->getNodeType() == Symbol::DECLARATION_PART){
        decl_node = buildDeclNode(children[1]);
    } else return nullptr;
    if(children[2]->getNodeType() == Symbol::COMPOUND_STATEMENT){
        comp_node = buildCompoundNode(children[2]);
    } else return nullptr;
    return std::make_shared<ProgramNode>(name, decl_node, comp_node);
}