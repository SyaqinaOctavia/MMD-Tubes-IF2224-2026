#include "ASTer.hpp"
#include "AST.hpp"
#include "../Symbol.hpp"
#include <string>
#include <functional>

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
    if (!root || root->getNodeType() != Symbol::VARIABLE)
        return nullptr;

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

    if (children.empty() || children[0]->getNodeType() != Symbol::ident)
        return nullptr;

    // Only when no component-variables 
    // the grammar: <variable> -> ident + (component-variable)*
    if (children.size() == 1)
        return std::make_shared<VarRefNode>(children[0]->getValue());

    return nullptr;
}
std::shared_ptr<ArrayAccessNode> ASTer::buildArrayAccessNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::VARIABLE)
        return nullptr;

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

    if (children.empty() || children[0]->getNodeType() != Symbol::ident) return nullptr;
    if (children.size() < 2) return nullptr;
    std::shared_ptr<TreeNode> lastComp = children.back();
    if (!lastComp || lastComp->getNodeType() != Symbol::COMPONENT_VARIABLE)
        return nullptr;
    std::vector<std::shared_ptr<TreeNode>> lastChildren = lastComp->getChildren();
    if (lastChildren.empty() || lastChildren[0]->getNodeType() != Symbol::lbrack)
        return nullptr;

    // save ident
    std::shared_ptr<ExprNode> current = std::make_shared<VarRefNode>(children[0]->getValue());
    for (std::size_t i = 1; i < children.size(); ++i) {
        std::shared_ptr<TreeNode> compVar = children[i];
        if (compVar->getNodeType() != Symbol::COMPONENT_VARIABLE) return nullptr;
        std::vector<std::shared_ptr<TreeNode>> cvChildren = compVar->getChildren();
        if (cvChildren.empty()) return nullptr;

        // the grammar: <component-variable> -> (lbrack + index-list + rbrack) | (period + ident)
        if (cvChildren[0]->getNodeType() == Symbol::lbrack) {
            if (cvChildren.size() < 2) return nullptr;
            std::shared_ptr<TreeNode> indexList = cvChildren[1];
            if (!indexList || indexList->getNodeType() != Symbol::INDEX_LIST) return nullptr;

            // Fungsi untuk mengambil semua index dari parse tree INDEX_LIST
            std::function<void(std::shared_ptr<TreeNode>,std::vector<std::shared_ptr<ExprNode>>&)>
            collectIndices = [&](std::shared_ptr<TreeNode> idxNode,
                                 std::vector<std::shared_ptr<ExprNode>>& out) {
                std::vector<std::shared_ptr<TreeNode>> idxChildren = idxNode->getChildren();
                for (std::shared_ptr<TreeNode>& ch : idxChildren) {
                    Symbol t = ch->getNodeType();
                    if (t == Symbol::intcon)
                        out.push_back(std::make_shared<LiteralNode>(
                            LiteralKind::Int, ch->getValue()));
                    else if (t == Symbol::charcon)
                        out.push_back(std::make_shared<LiteralNode>(
                            LiteralKind::Char, ch->getValue()));
                    else if (t == Symbol::ident)
                        out.push_back(
                            std::make_shared<VarRefNode>(ch->getValue()));
                    else if (t == Symbol::INDEX_LIST)
                        collectIndices(ch, out);
                    else if (t == Symbol::EXPRESSION) {
                        std::shared_ptr<ExprNode> expr = buildExprNode(ch);
                        if (expr) out.push_back(expr);
                    }
                }
            };

            std::vector<std::shared_ptr<ExprNode>> indices;
            collectIndices(indexList, indices);

            for (std::size_t j = 0; j < indices.size(); ++j) {
                current = std::make_shared<ArrayAccessNode>(current, indices[j]);
            }
        } else if (cvChildren[0]->getNodeType() == Symbol::period) {
            if (cvChildren.size() < 2 ||
                cvChildren[1]->getNodeType() != Symbol::ident) return nullptr;

            current = std::make_shared<FieldAccessNode>(
                current, cvChildren[1]->getValue());
        } else {
            return nullptr;
        }
    }

    return std::dynamic_pointer_cast<ArrayAccessNode>(current);
}
std::shared_ptr<FieldAccessNode> ASTer::buildFieldAccessNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::VARIABLE)
        return nullptr;

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

    if (children.empty() || children[0]->getNodeType() != Symbol::ident) return nullptr;
    if (children.size() < 2) return nullptr;

    std::shared_ptr<TreeNode> lastComp = children.back();
    if (lastComp->getNodeType() != Symbol::COMPONENT_VARIABLE)
        return nullptr;
    std::vector<std::shared_ptr<TreeNode>> lcChildren = lastComp->getChildren();
    if (lcChildren.empty() ||
        lcChildren[0]->getNodeType() != Symbol::period)
        return nullptr;

    std::shared_ptr<ExprNode> current = std::make_shared<VarRefNode>(children[0]->getValue());

    for (std::size_t i = 1; i < children.size(); ++i) {
        std::shared_ptr<TreeNode> compVar = children[i];
        if (compVar->getNodeType() != Symbol::COMPONENT_VARIABLE) return nullptr;

        std::vector<std::shared_ptr<TreeNode>> cvChildren = compVar->getChildren();
        if (cvChildren.empty()) return nullptr;

        if (cvChildren[0]->getNodeType() == Symbol::lbrack) {
            if (cvChildren.size() < 2) return nullptr;
            std::shared_ptr<TreeNode> indexList = cvChildren[1];
            if (!indexList ||
                indexList->getNodeType() != Symbol::INDEX_LIST)
                return nullptr;

            std::function<void(std::shared_ptr<TreeNode>,std::vector<std::shared_ptr<ExprNode>>&)>
            collectIndices = [&](std::shared_ptr<TreeNode> idxNode,
                                 std::vector<std::shared_ptr<ExprNode>>& out) {
                std::vector<std::shared_ptr<TreeNode>> idxChildren = idxNode->getChildren();
                for (std::shared_ptr<TreeNode>& ch : idxChildren) {
                    Symbol t = ch->getNodeType();
                    if (t == Symbol::intcon)
                        out.push_back(std::make_shared<LiteralNode>(
                            LiteralKind::Int, ch->getValue()));
                    else if (t == Symbol::charcon)
                        out.push_back(std::make_shared<LiteralNode>(
                            LiteralKind::Char, ch->getValue()));
                    else if (t == Symbol::ident)
                        out.push_back(
                            std::make_shared<VarRefNode>(ch->getValue()));
                    else if (t == Symbol::INDEX_LIST)
                        collectIndices(ch, out);
                    else if (t == Symbol::EXPRESSION) {
                        std::shared_ptr<ExprNode> expr = buildExprNode(ch);
                        if (expr) out.push_back(expr);
                    }
                }
            };

            std::vector<std::shared_ptr<ExprNode>> indices;
            collectIndices(indexList, indices);
            for (std::size_t j = 0; j < indices.size(); ++j) {
                current = std::make_shared<ArrayAccessNode>(current, indices[j]);
            }

        } else if (cvChildren[0]->getNodeType() == Symbol::period) {
            if (cvChildren.size() < 2 ||
                cvChildren[1]->getNodeType() != Symbol::ident)
                return nullptr;
            current = std::make_shared<FieldAccessNode>(
                current, cvChildren[1]->getValue());
        } else {
            return nullptr;
        }
    }

    return std::dynamic_pointer_cast<FieldAccessNode>(current);
}
std::shared_ptr<AssignNode> ASTer::buildAssignNode(std::shared_ptr<TreeNode> root) const {
     if (!root || root->getNodeType() != Symbol::ASSIGNMENT_STATEMENT)
        return nullptr;

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

    if (children.size() < 3) return nullptr;
    if (children[0]->getNodeType() != Symbol::VARIABLE) return nullptr;
    if (children[1]->getNodeType() != Symbol::becomes) return nullptr;
    if (children[2]->getNodeType() != Symbol::EXPRESSION) return nullptr;

    // assignment target from VARIABLE
    std::shared_ptr<TreeNode> varNode = children[0];
    std::vector<std::shared_ptr<TreeNode>> varChildren = varNode->getChildren();

    std::shared_ptr<ExprNode> target;

    if (varChildren.size() == 1) {
        // Simple variable: ident
        target = buildVarRefNode(varNode);
    } else {
        // Has component-variable
        std::shared_ptr<TreeNode> lastComp = varChildren.back();

        if (lastComp->getNodeType() != Symbol::COMPONENT_VARIABLE)
            return nullptr;
        std::vector<std::shared_ptr<TreeNode>> lastChildren = lastComp->getChildren();
        if (lastChildren.empty())
            return nullptr;
        if (lastChildren[0]->getNodeType() == Symbol::period) {
            target = buildFieldAccessNode(varNode);
        } else if (lastChildren[0]->getNodeType() == Symbol::lbrack) {
            target = buildArrayAccessNode(varNode);
        } else {
            return nullptr;
        }
    }

    if (!target) return nullptr;

    // assignment value from EXPRESSION
    std::shared_ptr<ExprNode> value = buildExprNode(children[2]);
    if (!value) return nullptr;

    return std::make_shared<AssignNode>(target, value);
}
std::shared_ptr<IfNode> ASTer::buildIfNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::IF_STATEMENT)
        return nullptr;

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();
    if (children.size() < 4) return nullptr;
    if (children[0]->getNodeType() != Symbol::ifsy)       return nullptr;
    if (children[1]->getNodeType() != Symbol::EXPRESSION) return nullptr;
    if (children[2]->getNodeType() != Symbol::thensy)     return nullptr;
    if (children[3]->getNodeType() != Symbol::STATEMENT)  return nullptr;

    std::shared_ptr<ExprNode> condition = buildExprNode(children[1]);
    if (!condition) return nullptr;

    std::shared_ptr<StmtNode> thenBlock = buildStmtNode(children[3]);
    if (!thenBlock) return nullptr;

    std::shared_ptr<StmtNode> elseBlock = nullptr;
    if (children.size() >= 6 &&
        children[4]->getNodeType() == Symbol::elsesy &&
        children[5]->getNodeType() == Symbol::STATEMENT) {
        elseBlock = buildStmtNode(children[5]);
    }

    return std::make_shared<IfNode>(condition, thenBlock, elseBlock);
}
std::shared_ptr<StmtNode> ASTer::buildStmtNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::STATEMENT) {
        return nullptr;
    }

    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

    // empty statement
    if (children.empty()) {
        return std::make_shared<CompoundNode>(std::vector<std::shared_ptr<StmtNode>>{});
    }
    std::shared_ptr<TreeNode> inner = children[0];
    switch (inner->getNodeType()) {
        case Symbol::ASSIGNMENT_STATEMENT:
            return buildAssignNode(inner);
        case Symbol::IF_STATEMENT:
            return buildIfNode(inner);
        case Symbol::WHILE_STATEMENT:
            return buildWhileNode(inner);
        case Symbol::REPEAT_STATEMENT:
            return buildRepeatNode(inner);
        case Symbol::FOR_STATEMENT:
            return buildForNode(inner);
        case Symbol::CASE_STATEMENT:
            return buildCaseNode(inner);
        case Symbol::PROCEDURE_FUNCTION_CALL: {
            std::shared_ptr<CallNode> callNode = buildCallNode(inner);
            if (!callNode) return nullptr;
            return std::make_shared<CallStmtNode>(callNode);
        }
        default:
            return nullptr;
    }
}
std::shared_ptr<ExprNode> ASTer::buildExprNode(std::shared_ptr<TreeNode> root) const {
    if (!root) return nullptr;

    // expression
    if (root->getNodeType() == Symbol::EXPRESSION) {
        std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

        if (children.empty()) return nullptr;
        std::shared_ptr<ExprNode> lhs = buildExprNode(children[0]);
        if (!lhs) return nullptr;
        if (children.size() == 3) {
            std::vector<std::shared_ptr<TreeNode>> opChildren = children[1]->getChildren();
            if (opChildren.empty()) return nullptr;
            std::string op = toString(opChildren[0]->getNodeType());
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[2]);
            if (!rhs) return nullptr;

            return std::make_shared<BinaryOpNode>(op, lhs, rhs);
        }

        return lhs;
    }

    // simple_expression
    if (root->getNodeType() == Symbol::SIMPLE_EXPRESSION) {
        std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

        if (children.empty()) return nullptr;

        int idx = 0;
        std::string unaryOp;

        if (children[idx]->getNodeType() == Symbol::plus ||
            children[idx]->getNodeType() == Symbol::minus) {
            unaryOp = toString(children[idx]->getNodeType());
            idx++;
        }

        if (idx >= static_cast<int>(children.size())) return nullptr;
        std::shared_ptr<ExprNode> current = buildExprNode(children[idx]);
        idx++;
        if (!current) return nullptr;
        if (!unaryOp.empty()) {
            current = std::make_shared<UnaryOpNode>(unaryOp, current);
        }

        while (idx + 1 < static_cast<int>(children.size()) &&
               children[idx]->getNodeType() == Symbol::ADDITIVE_OPERATOR) {
            std::vector<std::shared_ptr<TreeNode>> opChildren = children[idx]->getChildren();
            if (opChildren.empty()) return nullptr;
            std::string op = toString(opChildren[0]->getNodeType());
            idx++;
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[idx]);
            idx++;
            if (!rhs) return nullptr;
            current = std::make_shared<BinaryOpNode>(op, current, rhs);
        }

        return current;
    }

    // term
    if (root->getNodeType() == Symbol::TERM) {
        std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

        if (children.empty()) return nullptr;

        std::shared_ptr<ExprNode> current = buildExprNode(children[0]);
        if (!current) return nullptr;

        int idx = 1;

        while (idx + 1 < static_cast<int>(children.size()) &&
               children[idx]->getNodeType() == Symbol::MULTIPLICATIVE_OPERATOR) {
            std::vector<std::shared_ptr<TreeNode>> opChildren = children[idx]->getChildren();
            if (opChildren.empty()) return nullptr;
            std::string op = toString(opChildren[0]->getNodeType());
            idx++;
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[idx]);
            idx++;
            if (!rhs) return nullptr;
            current = std::make_shared<BinaryOpNode>(op, current, rhs);
        }

        return current;
    }

    // factor
    if (root->getNodeType() == Symbol::FACTOR) {
        std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();

        if (children.empty()) return nullptr;

        Symbol first = children[0]->getNodeType();

        if (first == Symbol::intcon) {
            return std::make_shared<LiteralNode>(LiteralKind::Int, children[0]->getValue());
        }
        if (first == Symbol::realcon) {
            return std::make_shared<LiteralNode>(LiteralKind::Real, children[0]->getValue());
        }
        if (first == Symbol::charcon) {
            return std::make_shared<LiteralNode>(LiteralKind::Char, children[0]->getValue());
        }
        if (first == Symbol::string) {
            return std::make_shared<LiteralNode>(LiteralKind::String, children[0]->getValue());
        }
        if (first == Symbol::notsy) {
            if (children.size() < 2) return nullptr;
            std::shared_ptr<ExprNode> operand = buildExprNode(children[1]);
            if (!operand) return nullptr;
            return std::make_shared<UnaryOpNode>("not", operand);
        }
        if (first == Symbol::lparent) {
            if (children.size() < 3) return nullptr;
            return buildExprNode(children[1]);
        }
        if (first == Symbol::PROCEDURE_FUNCTION_CALL) {
            return buildCallNode(children[0]);
        }
        // VARIABLE -> ident (COMPONENT_VARIABLE)*
        if (first == Symbol::VARIABLE) {
            std::vector<std::shared_ptr<TreeNode>> varChildren = children[0]->getChildren();
            if (varChildren.empty()) return nullptr;
            if (varChildren.size() == 1) {
                return buildVarRefNode(children[0]);
            }
            std::shared_ptr<TreeNode> lastComp = varChildren.back();
            std::vector<std::shared_ptr<TreeNode>> lastChildren = lastComp->getChildren();
            if (lastChildren.empty()) return nullptr;
            if (lastChildren[0]->getNodeType() == Symbol::period) {
                return buildFieldAccessNode(children[0]);
            }
            if (lastChildren[0]->getNodeType() == Symbol::lbrack) {
                return buildArrayAccessNode(children[0]);
            }

            return nullptr;
        }

        return nullptr;
    }

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