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
std::vector<std::shared_ptr<DeclNode>> ASTer::buildDeclNodes(std::shared_ptr<TreeNode> root) const {
    std::vector<std::shared_ptr<DeclNode>> declList;
    if (root->getNodeType() != Symbol::DECLARATION_PART || root->getChildren().empty())
        return declList;

    for (auto child : root->getChildren()){
        switch (child->getNodeType()) {
            case Symbol::CONST_DECLARATION:         {
                auto constDecls = buildConstDeclNode(child);
                declList.insert(declList.end(), constDecls.begin(), constDecls.end());
            }
            case Symbol::TYPE_DECLARATION:          {
                auto typeDecls = buildTypeDeclNode(child);
                declList.insert(declList.end(), typeDecls.begin(), typeDecls.end());
            }
            case Symbol::VAR_DECLARATION:           {
                auto varDecls = buildVarDeclNode(child);
                declList.insert(declList.end(), varDecls.begin(), varDecls.end());
            }
            // case Symbol::PARAMETER_GROUP:           declList.push_back(buildParamDeclNode(child, false));
            case Symbol::PROCEDURE_DECLARATION:     declList.push_back(buildProcDeclNode(child));
            case Symbol::FUNCTION_DECLARATION:      declList.push_back(buildFuncDeclNode(child));
        }
    }
}
std::vector<std::shared_ptr<ConstDeclNode>> ASTer::buildConstDeclNode(std::shared_ptr<TreeNode> root) const {
    // root = const-declaration: constsy + (ident + eql + constant + semicolon)+
    std::vector<std::shared_ptr<ConstDeclNode>> decls;
    auto children = root->getChildren();

    // skip children[0] = constsy, then stride 4: ident + eql + constant + semicolon
    for (size_t i = 1; i + 2 < children.size(); i += 4) {
        std::string name = children[i]->getValue();
        auto value = constantToLiteral(children[i + 2]);
        decls.push_back(std::make_shared<ConstDeclNode>(name, value));
    }

    return decls;
}
std::vector<std::shared_ptr<TypeDeclNode>> ASTer::buildTypeDeclNode(std::shared_ptr<TreeNode> root) const {
    // root = type-declaration: typesy + (ident + eql + type + semicolon)+
    std::vector<std::shared_ptr<TypeDeclNode>> decls;
    auto children = root->getChildren();

    // skip children[0] = typesy, then stride 4: ident + eql + type + semicolon
    for (size_t i = 1; i + 2 < children.size(); i += 4) {
        std::string name               = children[i]->getValue();
        std::shared_ptr<TypeNode> type = buildTypeNode(children[i + 2]);
        decls.push_back(std::make_shared<TypeDeclNode>(name, type));
    }

    return decls;
}
std::vector<std::shared_ptr<VarDeclNode>> ASTer::buildVarDeclNode(std::shared_ptr<TreeNode> root) const {
    // root = var-declaration: varsy + (identifier-list + colon + type + semicolon)+
    std::vector<std::shared_ptr<VarDeclNode>> decls;
    auto children = root->getChildren();

    // skip children[0] = varsy, then stride 4: identifier-list + colon + type + semicolon
    for (size_t i = 1; i + 2 < children.size(); i += 4) {
        std::vector<std::string> names   = identifierListToStrings(children[i]);
        std::shared_ptr<TypeNode> type   = buildTypeNode(children[i + 2]);
        decls.push_back(std::make_shared<VarDeclNode>(names, type));
    }

    return decls;
}
std::shared_ptr<ParamDeclNode> ASTer::buildParamDeclNode(std::shared_ptr<TreeNode> root) const {
    // parameter-group: identifier-list + colon + (ident | array-type)
    auto children = root->getChildren();
    std::vector<std::string> names = identifierListToStrings(children[0]);
    std::shared_ptr<TreeNode> typeChild = children[2];
    std::shared_ptr<TypeNode> type;
    if (typeChild->getNodeType() == Symbol::ident) {
        type = buildSimpleType(typeChild);
    } else if (typeChild->getNodeType() == Symbol::ARRAY_TYPE) {
        type = buildArrayType(typeChild);
    }
    return std::make_shared<ParamDeclNode>(names, type, false); // is_var always false
}
std::vector<std::shared_ptr<ParamDeclNode>> ASTer::buildFormalParams(std::shared_ptr<TreeNode> root) const {
    // lparent + parameter-group + (semicolon + parameter-group)* + rparent
    std::vector<std::shared_ptr<ParamDeclNode>> params;
    for (auto& child : root->getChildren()) {
        if (child->getNodeType() == Symbol::PARAMETER_GROUP) {
            params.push_back(buildParamDeclNode(child));
        }
    }
    return params;
}
std::shared_ptr<ProcDeclNode> ASTer::buildProcDeclNode(std::shared_ptr<TreeNode> root) const {
    // proceduresy + ident + (formal-parameter-list)? + semicolon + block + semicolon
    auto children = root->getChildren();
    std::string name = children[1]->getValue();                         // ident
    int offset = 2;
    std::vector<std::shared_ptr<ParamDeclNode>> params;
    if (children[offset]->getNodeType() == Symbol::FORMAL_PARAMETER_LIST) {
        params = buildFormalParams(children[offset]);
        offset++;
    }
    // children[offset] = semicolon (skip)
    offset++;
    // block = declaration-part + compound-statement
    std::shared_ptr<TreeNode> block = children[offset];
    auto blockChildren = block->getChildren();

    std::vector<std::shared_ptr<DeclNode>> local_var = buildDeclNodes(blockChildren[0]);
    std::shared_ptr<StmtNode> body = buildCompoundNode(blockChildren[1]);

    return std::make_shared<ProcDeclNode>(name, params, local_var, body);
}

std::shared_ptr<FuncDeclNode> ASTer::buildFuncDeclNode(std::shared_ptr<TreeNode> root) const {
    // functionsy + ident + (formal-parameter-list)? + colon + ident + semicolon + block + semicolon
    auto children = root->getChildren();
    std::string name = children[1]->getValue();                         // ident

    int offset = 2;
    std::vector<std::shared_ptr<ParamDeclNode>> params;
    if (children[offset]->getNodeType() == Symbol::FORMAL_PARAMETER_LIST) {
        params = buildFormalParams(children[offset]);
        offset++;
    }
    // children[offset] = colon (skip)
    offset++;
    std::shared_ptr<TypeNode> return_type = buildSimpleType(children[offset]); // ident
    offset++;
    // children[offset] = semicolon (skip)
    offset++;
    // block = declaration-part + compound-statement
    std::shared_ptr<TreeNode> block = children[offset];
    auto blockChildren = block->getChildren();

    std::vector<std::shared_ptr<DeclNode>> local_var = buildDeclNodes(blockChildren[0]);
    std::shared_ptr<StmtNode> body = buildCompoundNode(blockChildren[1]);
    return std::make_shared<FuncDeclNode>(name, return_type, params, local_var, body);
}
std::shared_ptr<LiteralNode> ASTer::buildLiteralNode(std::shared_ptr<TreeNode> root) const {
    if (!root) return nullptr;
    Symbol sym = root->getNodeType();
    if (sym == Symbol::intcon)  return std::make_shared<LiteralNode>(LiteralKind::Int,    root->getValue());
    if (sym == Symbol::realcon) return std::make_shared<LiteralNode>(LiteralKind::Real,   root->getValue());
    if (sym == Symbol::charcon) return std::make_shared<LiteralNode>(LiteralKind::Char,   root->getValue());
    if (sym == Symbol::string)  return std::make_shared<LiteralNode>(LiteralKind::String, root->getValue());
    return nullptr;
}
std::shared_ptr<CallNode> ASTer::buildCallNode(std::shared_ptr<TreeNode> root) const {
    // PROCEDURE_FUNCTION_CALL: ident + (lparent + parameter-list? + rparent)?
    if (!root) return nullptr;
    auto children = root->getChildren();

    std::string name = children[0]->getValue();
    std::vector<std::shared_ptr<ExprNode>> args;

    // has argument list: ident + lparent + (expression + (comma + expression)*)? + rparent
    if (children.size() > 1) {
        for (auto& child : children) {
            if (child->getNodeType() == Symbol::EXPRESSION) {
                args.push_back(buildExprNode(child));
            }
        }
    }

    return std::make_shared<CallNode>(name, args);
}
std::shared_ptr<BinaryOpNode> ASTer::buildBinaryOpNode(std::shared_ptr<ExprNode> lhs, std::shared_ptr<TreeNode> opNode, std::shared_ptr<ExprNode> rhs) const {
    if (!lhs || !opNode || !rhs) return nullptr;
    auto opChildren = opNode->getChildren();
    if (opChildren.empty()) return nullptr;
    std::string op = toString(opChildren[0]->getNodeType());
    return std::make_shared<BinaryOpNode>(op, lhs, rhs);
}
std::shared_ptr<UnaryOpNode> ASTer::buildUnaryOpNode(std::shared_ptr<TreeNode> opNode, std::shared_ptr<ExprNode> operand) const {
    if (!opNode || !operand) return nullptr;
    std::string op = toString(opNode->getNodeType());
    return std::make_shared<UnaryOpNode>(op, operand);
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
        case Symbol::COMPOUND_STATEMENT:
            return buildCompoundNode(inner);
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

    // expression: simple-expression (relational-operator + simple-expression)?
    if (root->getNodeType() == Symbol::EXPRESSION) {
        auto children = root->getChildren();
        if (children.empty()) return nullptr;

        std::shared_ptr<ExprNode> lhs = buildExprNode(children[0]);
        if (!lhs) return nullptr;

        if (children.size() == 3) {
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[2]);
            if (!rhs) return nullptr;
            return buildBinaryOpNode(lhs, children[1], rhs);
        }

        return lhs;
    }

    // simple-expression: (plus | minus)? term (additive-operator + term)*
    if (root->getNodeType() == Symbol::SIMPLE_EXPRESSION) {
        auto children = root->getChildren();
        if (children.empty()) return nullptr;

        int idx = 0;
        std::shared_ptr<ExprNode> current;

        // optional unary sign
        if (children[idx]->getNodeType() == Symbol::plus ||
            children[idx]->getNodeType() == Symbol::minus) {
            std::shared_ptr<ExprNode> operand = buildExprNode(children[idx + 1]);
            if (!operand) return nullptr;
            current = buildUnaryOpNode(children[idx], operand);
            idx += 2;
        } else {
            current = buildExprNode(children[idx]);
            if (!current) return nullptr;
            idx++;
        }

        // additive-operator + term pairs
        while (idx + 1 < static_cast<int>(children.size()) &&
               children[idx]->getNodeType() == Symbol::ADDITIVE_OPERATOR) {
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[idx + 1]);
            if (!rhs) return nullptr;
            current = buildBinaryOpNode(current, children[idx], rhs);
            idx += 2;
        }

        return current;
    }

    // term: factor (multiplicative-operator + factor)*
    if (root->getNodeType() == Symbol::TERM) {
        auto children = root->getChildren();
        if (children.empty()) return nullptr;

        std::shared_ptr<ExprNode> current = buildExprNode(children[0]);
        if (!current) return nullptr;

        int idx = 1;
        while (idx + 1 < static_cast<int>(children.size()) &&
               children[idx]->getNodeType() == Symbol::MULTIPLICATIVE_OPERATOR) {
            std::shared_ptr<ExprNode> rhs = buildExprNode(children[idx + 1]);
            if (!rhs) return nullptr;
            current = buildBinaryOpNode(current, children[idx], rhs);
            idx += 2;
        }

        return current;
    }

    // factor
    if (root->getNodeType() == Symbol::FACTOR) {
        auto children = root->getChildren();
        if (children.empty()) return nullptr;

        Symbol first = children[0]->getNodeType();

        if (first == Symbol::intcon  ||
            first == Symbol::realcon ||
            first == Symbol::charcon ||
            first == Symbol::string) {
            return buildLiteralNode(children[0]);
        }
        if (first == Symbol::notsy) {
            if (children.size() < 2) return nullptr;
            std::shared_ptr<ExprNode> operand = buildExprNode(children[1]);
            return buildUnaryOpNode(children[0], operand);
        }
        if (first == Symbol::lparent) {
            if (children.size() < 3) return nullptr;
            return buildExprNode(children[1]);
        }
        if (first == Symbol::PROCEDURE_FUNCTION_CALL) {
            return buildCallNode(children[0]);
        }
        if (first == Symbol::VARIABLE) {
            auto varChildren = children[0]->getChildren();
            if (varChildren.empty()) return nullptr;
            if (varChildren.size() == 1) {
                return buildVarRefNode(children[0]);
            }
            std::shared_ptr<TreeNode> lastComp = varChildren.back();
            auto lastChildren = lastComp->getChildren();
            if (lastChildren.empty()) return nullptr;
            if (lastChildren[0]->getNodeType() == Symbol::period) {
                return buildFieldAccessNode(children[0]);
            }
            if (lastChildren[0]->getNodeType() == Symbol::lbrack) {
                return buildArrayAccessNode(children[0]);
            }
        }

        return nullptr;
    }

    return nullptr;
}

std::shared_ptr<WhileNode>ASTer::buildWhileNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::WHILE_STATEMENT) return nullptr;
    auto children = root->getChildren();
    if (children.size() < 4) return nullptr;
    // whilesy + EXPRESSION + dosy + COMPOUND_STATEMENT
    auto condition = buildExprNode(children[1]);
    auto body = buildCompoundNode(children[3]);
    if (!condition || !body) return nullptr;
    return std::make_shared<WhileNode>(condition, body);
}

std::shared_ptr<ForNode> ASTer::buildForNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::FOR_STATEMENT) return nullptr;
    auto children = root->getChildren();
    if (children.size() < 8) return nullptr;
    // forsy + ident + becomes + EXPRESSION + (tosy|downtosy) + EXPRESSION + dosy + COMPOUND + semicolon
    std::string var = children[1]->getValue();
    bool goesUp = (children[4]->getNodeType() == Symbol::tosy);
    auto start = buildExprNode(children[3]);
    auto end = buildExprNode(children[5]);
    auto body = buildCompoundNode(children[7]);
    if (!start || !end || !body) return nullptr;
    return std::make_shared<ForNode>(goesUp, var, start, end, body);
}

std::shared_ptr<RepeatNode>
ASTer::buildRepeatNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::REPEAT_STATEMENT) return nullptr;
    auto children = root->getChildren();
    if (children.size() < 4) return nullptr;
    
    // repeatsy + STATEMENT_LIST + untilsy + EXPRESSION 
    auto stmtListNode = children[1];
    std::vector<std::shared_ptr<StmtNode>> stmts;
    
    // Statement list
    if (stmtListNode && stmtListNode->getNodeType() == Symbol::STATEMENT_LIST) {
        for (auto& child : stmtListNode->getChildren()) {
            if (child->getNodeType() == Symbol::STATEMENT) {
                auto stmt = buildStmtNode(child);
                if (stmt) stmts.push_back(stmt);
            }
        }
    }
    
    auto body = std::make_shared<CompoundNode>(stmts);
    auto cond = buildExprNode(children[3]);
    
    if (!body || !cond) return nullptr;
    
    return std::make_shared<RepeatNode>(body, cond);
}

std::shared_ptr<CaseNode>ASTer::buildCaseNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::CASE_STATEMENT) return nullptr;
    auto children = root->getChildren();
    
    // casesy + EXPRESSION + ofsy + CASE_BLOCK + endsy
    if (children.size() < 4) return nullptr;
    
    auto key = buildExprNode(children[1]);
    if (!key) return nullptr;
    
    std::vector<std::pair<std::vector<std::shared_ptr<ExprNode>>, std::shared_ptr<StmtNode>>> cases;

    std::shared_ptr<TreeNode> currentBlock = children[3];
    
    while (currentBlock && currentBlock->getNodeType() == Symbol::CASE_BLOCK) {
        auto blockChildren = currentBlock->getChildren();
        std::vector<std::shared_ptr<ExprNode>> keys;
        size_t i = 0;
        
        while (i < blockChildren.size() && blockChildren[i]->getNodeType() == Symbol::CONSTANT) {
            keys.push_back(constantToLiteral(blockChildren[i]));
            i++;
            if (i < blockChildren.size() && blockChildren[i]->getNodeType() == Symbol::comma) {
                i++;
            }
        }
        
        if (i < blockChildren.size() && blockChildren[i]->getNodeType() == Symbol::colon) {
            i++;
        }
        
        if (i < blockChildren.size() && blockChildren[i]->getNodeType() == Symbol::STATEMENT) {
            auto stmt = buildStmtNode(blockChildren[i]);
            cases.push_back({keys, stmt});
            i++;
        }
        
        std::shared_ptr<TreeNode> nextBlock = nullptr;
        while (i < blockChildren.size()) {
            if (blockChildren[i]->getNodeType() == Symbol::semicolon) {
                i++;
                if (i < blockChildren.size() && blockChildren[i]->getNodeType() == Symbol::CASE_BLOCK) {
                    nextBlock = blockChildren[i];
                    break;
                }
            } else {
                i++;
            }
        }
        
        currentBlock = nextBlock;
    }
    
    return std::make_shared<CaseNode>(key, cases);
}

std::shared_ptr<CompoundNode>
ASTer::buildCompoundNode(std::shared_ptr<TreeNode> root) const {
    if (!root || root->getNodeType() != Symbol::COMPOUND_STATEMENT) return nullptr;
    auto children = root->getChildren();
    // beginsy + STATEMENT_LIST + endsy
    if (children.size() < 2) return nullptr;
    for (auto& child : children) {
        if (child->getNodeType() == Symbol::STATEMENT_LIST)
            return buildStatementList(child);
    })
    return std::make_shared<CompoundNode>(std::vector<std::shared_ptr<StmtNode>>{});
}

std::shared_ptr<CompoundNode> ASTer::buildCompoundNode(std::shared_ptr<TreeNode> root) const {
    return nullptr;
}
std::shared_ptr<ProgramNode> ASTer::buildProgramNode(std::shared_ptr<TreeNode> root) const {
    std::vector<std::shared_ptr<TreeNode>> children = root->getChildren();
    std::string name;
    std::vector<std::shared_ptr<DeclNode>> decl_nodes;
    std::shared_ptr<CompoundNode> comp_node;
    if(children[0]->getNodeType() == Symbol::PROGRAM_HEADER){
        std::vector<std::shared_ptr<TreeNode>> grandchildren = children[0]->getChildren();
        name = grandchildren[1]->getValue();
    } else return nullptr;
    if(children[1]->getNodeType() == Symbol::DECLARATION_PART){
        decl_nodes = buildDeclNodes(children[1]);
    } else return nullptr;
    if(children[2]->getNodeType() == Symbol::COMPOUND_STATEMENT){
        comp_node = buildCompoundNode(children[2]);
    } else return nullptr;
    return std::make_shared<ProgramNode>(name, decl_nodes, comp_node);
}

// Statement builders helper
std::shared_ptr<CompoundNode>
ASTer::buildStatementList(std::shared_ptr<TreeNode> root) const {
    // STATEMENT_LIST: STATEMENT (semicolon + STATEMENT)*
    std::vector<std::shared_ptr<StmtNode>> stmts;
    for (auto& child : root->getChildren()) {
        if (child->getNodeType() == Symbol::STATEMENT) {
            auto stmt = buildStmtNode(child);
            if (stmt) stmts.push_back(stmt);
        }
    }
    return std::make_shared<CompoundNode>(stmts);
}