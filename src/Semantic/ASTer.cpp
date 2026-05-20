#include "ASTer.hpp"
#include "AST.hpp"
#include "../Symbol.hpp"
#include <string>

std::shared_ptr<ASTNode> ASTer::buildAST(std::shared_ptr<TreeNode> root) const {
    return buildProgramNode(root);
}

std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>> ASTer::fieldPartToField(std::shared_ptr<TreeNode> node) const {
    // node = field-part: identifier-list + colon + type
    auto children = node->getChildren();
    
    std::vector<std::string> names = identifierListToStrings(children[0]);
    std::shared_ptr<TypeNode> type = buildTypeNode(children[2]); // skip colon at [1]
    
    return { names, type };
}

std::shared_ptr<TypeNode> ASTer::buildTypeNode(std::shared_ptr<TreeNode> root) const {
    if (root->getNodeType() != Symbol::TYPE || root->getChildren().empty())
        return nullptr;

    std::shared_ptr<TreeNode> child = root->getChildren()[0];
    switch (child->getNodeType()) {
        case Symbol::ident:       return buildSimpleType(child);
        case Symbol::ARRAY_TYPE:  return buildArrayType(child);
        case Symbol::RANGE:       return buildRangeType(child);
        case Symbol::ENUMERATED:  return buildEnumType(child);
        case Symbol::RECORD_TYPE: return buildRecordType(child);
        default:                  return nullptr;
    }
}

std::shared_ptr<TypeNode> ASTer::buildSimpleType(std::shared_ptr<TreeNode> node) const {
    return std::make_shared<SimpleTypeNode>(node->getValue());
}

std::shared_ptr<TypeNode> ASTer::buildArrayIndex(std::shared_ptr<TreeNode> node) const {
    if (node->getNodeType() == Symbol::RANGE)
        return buildRangeType(node);
    if (node->getNodeType() == Symbol::ident)
        return buildSimpleType(node);
    return nullptr;
}

std::shared_ptr<TypeNode> ASTer::buildArrayType(std::shared_ptr<TreeNode> node) const {
    // arraysy + lbrack + (range | ident) + rbrack + ofsy + type
    auto gc = node->getChildren();
    std::shared_ptr<TypeNode> index   = buildArrayIndex(gc[2]);
    std::shared_ptr<TypeNode> element = buildTypeNode(gc[5]);
    return std::make_shared<ArrayTypeNode>(element, index);
}

std::shared_ptr<TypeNode> ASTer::buildRangeType(std::shared_ptr<TreeNode> node) const {
    // constant + period + period + constant
    auto gc = node->getChildren();
    if (gc[0]->getNodeType() != Symbol::CONSTANT) return nullptr;
    if (gc[3]->getNodeType() != Symbol::CONSTANT) return nullptr;
    auto low  = constantToLiteral(gc[0]);
    auto high = constantToLiteral(gc[3]);
    return std::make_shared<RangeTypeNode>(low, high);
}

std::shared_ptr<TypeNode> ASTer::buildEnumType(std::shared_ptr<TreeNode> node) const {
    return std::make_shared<EnumTypeNode>(enumToStrings(node));
}

std::shared_ptr<TypeNode> ASTer::buildRecordType(std::shared_ptr<TreeNode> node) const {
    // recordsy + field-list + endsy
    auto fieldList = node->getChildren()[1];
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> fields;
    for (auto child : fieldList->getChildren()) {
        if (child->getNodeType() == Symbol::FIELD_PART)
            fields.push_back(fieldPartToField(child));
    }
    return std::make_shared<FieldTypeNode>(fields);
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