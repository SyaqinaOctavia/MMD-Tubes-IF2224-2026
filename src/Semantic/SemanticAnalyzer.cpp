#include "SemanticAnalyzer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

void SemanticAnalyzer::visit(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    switch (node->getASTType()) {
        case ASTType::ProgramNode:   visitProgram  (std::dynamic_pointer_cast<ProgramNode> (node)); break;
        case ASTType::ConstDeclNode: visitConstDecl(std::dynamic_pointer_cast<ConstDeclNode>(node)); break;
        case ASTType::TypeDeclNode:  visitTypeDecl (std::dynamic_pointer_cast<TypeDeclNode> (node)); break;
        case ASTType::VarDeclNode:   visitVarDecl  (std::dynamic_pointer_cast<VarDeclNode>  (node)); break;
        case ASTType::ParamDeclNode: visitParamDecl(std::dynamic_pointer_cast<ParamDeclNode>(node)); break;
        case ASTType::ProcDeclNode:  visitProcDecl (std::dynamic_pointer_cast<ProcDeclNode> (node)); break;
        case ASTType::FuncDeclNode:  visitFuncDecl (std::dynamic_pointer_cast<FuncDeclNode> (node)); break;
        case ASTType::AssignNode:    visitAssign   (std::dynamic_pointer_cast<AssignNode>   (node)); break;
        case ASTType::IfNode:        visitIf       (std::dynamic_pointer_cast<IfNode>       (node)); break;
        case ASTType::WhileNode:     visitWhile    (std::dynamic_pointer_cast<WhileNode>    (node)); break;
        case ASTType::ForNode:       visitFor      (std::dynamic_pointer_cast<ForNode>      (node)); break;
        case ASTType::RepeatNode:    visitRepeat   (std::dynamic_pointer_cast<RepeatNode>   (node)); break;
        case ASTType::CaseNode:      visitCase     (std::dynamic_pointer_cast<CaseNode>     (node)); break;
        case ASTType::CompoundNode:  visitCompound (std::dynamic_pointer_cast<CompoundNode> (node)); break;
        case ASTType::CallStmtNode:  visitCallStmt (std::dynamic_pointer_cast<CallStmtNode> (node)); break;
        default: break;
    }
}

int SemanticAnalyzer::visitExpr(std::shared_ptr<ExprNode> node) {
    if (!node) return T_NONE;
    int t = T_NONE;
    switch (node->getASTType()) {
        case ASTType::LiteralNode:     t = visitLiteral    (std::dynamic_pointer_cast<LiteralNode>    (node)); break;
        case ASTType::VarRefNode:      t = visitVarRef     (std::dynamic_pointer_cast<VarRefNode>     (node)); break;
        case ASTType::BinaryOpNode:    t = visitBinaryOp   (std::dynamic_pointer_cast<BinaryOpNode>   (node)); break;
        case ASTType::UnaryOpNode:     t = visitUnaryOp    (std::dynamic_pointer_cast<UnaryOpNode>    (node)); break;
        case ASTType::CallNode:        t = visitCall       (std::dynamic_pointer_cast<CallNode>       (node)); break;
        case ASTType::ArrayAccessNode: t = visitArrayAccess(std::dynamic_pointer_cast<ArrayAccessNode>(node)); break;
        case ASTType::FieldAccessNode: t = visitFieldAccess(std::dynamic_pointer_cast<FieldAccessNode>(node)); break;
        default: break;
    }
    return t;
}

int SemanticAnalyzer::visitType(std::shared_ptr<TypeNode> node) {
    if (!node) return T_NONE;
    switch (node->getKind()) {
        case TypeNode::Kind::Simple:     return visitSimpleType(std::dynamic_pointer_cast<SimpleTypeNode>(node));
        case TypeNode::Kind::Array:      return visitArrayType (std::dynamic_pointer_cast<ArrayTypeNode> (node));
        case TypeNode::Kind::Enumerated: return visitEnumType  (std::dynamic_pointer_cast<EnumTypeNode>  (node));
        case TypeNode::Kind::Range:      return visitRangeType (std::dynamic_pointer_cast<RangeTypeNode> (node));
        case TypeNode::Kind::Record:     return visitFieldType (std::dynamic_pointer_cast<FieldTypeNode> (node));
        default: return T_NONE;
    }
}

void SemanticAnalyzer::visitProgram(std::shared_ptr<ProgramNode> node) {
    if (!node) return;
    std::string name = node->getName();
    symTab.addTab(name, OBJ_PROC, T_NONE, 0, 1, 0);

    for (auto& decl : node->getDeclarations())
        visit(decl);

    if (node->getMain())
        visitCompound(node->getMain());
}

void SemanticAnalyzer::visitConstDecl(std::shared_ptr<ConstDeclNode> node) {
    if (!node) return;
    std::string name = node->getName();

    // Check for redeclaration
    if (symTab.searchCurrentScope(name) >= 0) {
        semanticError("Redeclaration of constant '" + node->getName() + "'");
        return;
    }

    // Infer type
    int t = visitExpr(node->getValue());

    // Extract string representation for const_value
    std::string cval;
    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(node->getValue()))
        cval = lit->getValue();

    int idx = symTab.addTab(name, OBJ_CONST, t, 0, 1, 0);

    // Store numeric value in adr
    symTab.getTab(idx).const_value = cval;
    if (t == T_INTEGER && !cval.empty()) {
        try { symTab.getTab(idx).adr = std::stoi(cval); }
        catch (...) {}
    }
}

void SemanticAnalyzer::visitTypeDecl(std::shared_ptr<TypeDeclNode> node) {
    if (!node) return;
    std::string name = node->getName();

    if (symTab.searchCurrentScope(name) >= 0) {
        semanticError("Redeclaration of type '" + node->getName() + "'");
        return;
    }

    lastTypeRef = 0;
    int t   = visitType(node->getTypeSpec());
    int ref = lastTypeRef;

    symTab.addTab(name, OBJ_TYPE, t, ref, 1, 0);
}

void SemanticAnalyzer::visitVarDecl(std::shared_ptr<VarDeclNode> node) {
    if (!node) return;
    lastTypeRef = 0;
    int t   = visitType(node->getType());
    int ref = lastTypeRef;

    for (const auto& rawName : node->getNames()) {
        std::string name = rawName;

        if (symTab.searchCurrentScope(name) >= 0) {
            semanticError("Redeclaration of variable '" + rawName + "' in current scope");
            continue;
        }

        int blockIdx = symTab.getCurrentBlock();
        int vsze = symTab.getBlockTab(blockIdx).vsze;
        symTab.addTab(name, OBJ_VAR, t, ref, 1, vsze);
        symTab.getBlockTab(blockIdx).vsze++;
    }
}

void SemanticAnalyzer::visitParamDecl(std::shared_ptr<ParamDeclNode> node) {
    if (!node) return;
    lastTypeRef = 0;
    int t   = visitType(node->getType());
    int ref = lastTypeRef;
    int nrm = node->isVarParam() ? 0 : 1; // 0=by-ref (var param), 1=by-value

    for (const auto& rawName : node->getNames()) {
        std::string name = rawName;

        if (symTab.searchCurrentScope(name) >= 0) {
            semanticError("Duplicate parameter name '" + rawName + "'");
            continue;
        }

        int blockIdx = symTab.getCurrentBlock();
        int psze = symTab.getBlockTab(blockIdx).psze;
        symTab.addTab(name, OBJ_VAR, t, ref, nrm, psze);
        symTab.getBlockTab(blockIdx).psze++;
    }
}


void SemanticAnalyzer::visitProcDecl(std::shared_ptr<ProcDeclNode> node) {
    if (!node) return;
    std::string name = node->getName();

    if (symTab.searchCurrentScope(name) >= 0) {
        semanticError("Redeclaration of procedure '" + node->getName() + "'");
        return;
    }

    // Add procedure to outer
    int procTabIdx = symTab.addTab(name, OBJ_PROC, T_NONE, 0, 1, 0);

    // Enter new scope for parameters and body
    symTab.enterScope();
    int blockIdx = symTab.getCurrentBlock();

    symTab.getTab(procTabIdx).ref = blockIdx;

    // Visit params
    for (auto& param : node->getParams())
        visitParamDecl(param);

    // Record lpar = last parameter index in this block
    symTab.getBlockTab(blockIdx).lpar = symTab.getBlockTab(blockIdx).last;

    // Visit local declarations
    for (auto& decl : node->getLocalVar())
        visit(decl);

    // Visit body
    if (node->getBody())
        visit(node->getBody());

    symTab.exitScope();
}

void SemanticAnalyzer::visitFuncDecl(std::shared_ptr<FuncDeclNode> node) {
    if (!node) return;
    std::string name = node->getName();

    if (symTab.searchCurrentScope(name) >= 0) {
        semanticError("Redeclaration of function '" + node->getName() + "'");
        return;
    }

    // Resolve return type
    lastTypeRef = 0;
    int retType = visitType(node->getReturnType());

    // Add function to the outer scope
    int funcTabIdx = symTab.addTab(name, OBJ_FUNC, retType, 0, 1, 0);

    // Enter new scope for the function
    symTab.enterScope();
    int blockIdx = symTab.getCurrentBlock();
    symTab.getTab(funcTabIdx).ref = blockIdx;

    // Visit params
    for (auto& param : node->getParams())
        visitParamDecl(param);

    symTab.getBlockTab(blockIdx).lpar = symTab.getBlockTab(blockIdx).last;

    // Visit local declarations and body
    for (auto& decl : node->getLocalVar())
        visit(decl);

    if (node->getBody())
        visit(node->getBody());

    symTab.exitScope();
}

// ========================== STATEMENT VISITORS ==================================

void SemanticAnalyzer::visitAssign(std::shared_ptr<AssignNode> node) {
    if (!node) return;
    int lhsType = visitExpr(node->getTarget());
    int rhsType = visitExpr(node->getValue());

    if (lhsType == T_NONE || rhsType == T_NONE) return;

    if (!isAssignmentCompatible(lhsType, rhsType))
        semanticError("Assignment incompatible: cannot assign "
                      + typeToString(rhsType) + " to " + typeToString(lhsType));
}

void SemanticAnalyzer::visitIf(std::shared_ptr<IfNode> node) {
    if (!node) return;
    int condType = visitExpr(node->getCondition());
    if (condType != T_BOOLEAN && condType != T_NONE) semanticError("IF condition must be Boolean, got " + typeToString(condType));
    visit(node->getThenBlock());
    if (node->getElseBlock()) visit(node->getElseBlock());
}

void SemanticAnalyzer::visitWhile(std::shared_ptr<WhileNode> node) {
    if (!node) return;
    int condType = visitExpr(node->getCondition());
    if (condType != T_BOOLEAN && condType != T_NONE)
        semanticError("WHILE condition must be Boolean, got " + typeToString(condType));
    visit(node->getBody());
}

void SemanticAnalyzer::visitFor(std::shared_ptr<ForNode> node) {
    if (!node) return;
    std::string varName = node->getMovingVar();
    int idx = symTab.searchTab(varName);
    if (idx < 0) {
        semanticError("FOR loop variable '" + node->getMovingVar() + "' is undeclared");
    } // TODO : check if move variable must be integer

    int startType = visitExpr(node->getStartPoint());
    int endType   = visitExpr(node->getEndPoint());
    visit(node->getBody());
}

void SemanticAnalyzer::visitRepeat(std::shared_ptr<RepeatNode> node) {
    if (!node) return;
    visit(node->getBody());
    int condType = visitExpr(node->getUntilCondition());
    if (condType != T_BOOLEAN && condType != T_NONE)
        semanticError("REPEAT-UNTIL condition must be Boolean, got " + typeToString(condType));
}

void SemanticAnalyzer::visitCase(std::shared_ptr<CaseNode> node) {
    if (!node) return;
    int keyType = visitExpr(node->getKey());

    // Case key must be ordinal (Integer, Char, Boolean)
    if (keyType != T_INTEGER && keyType != T_CHAR && keyType != T_BOOLEAN && keyType != T_NONE)
        semanticError("CASE key must be ordinal type (Integer, Char, or Boolean), got "
                      + typeToString(keyType));

    for (auto& [labels, stmt] : node->getCases()) {
        for (auto& label : labels) {
            int lt = visitExpr(label);
            if (!isCompatible(keyType, lt) && lt != T_NONE && keyType != T_NONE)
                semanticError("CASE label type incompatible with case expression");
        }
        if (stmt) visit(stmt);
    }
}

void SemanticAnalyzer::visitCompound(std::shared_ptr<CompoundNode> node) {
    if (!node) return;
    for (auto& stmt : node->getStatements())
        visit(stmt);
}

void SemanticAnalyzer::visitCallStmt(std::shared_ptr<CallStmtNode> node) {
    if (!node || !node->getCall()) return;
    visitCall(node->getCall());
}

// ========================== EXPRESSION VISITORS ==================================

int SemanticAnalyzer::visitLiteral(std::shared_ptr<LiteralNode> node) {
    if (!node) return T_NONE;
    int t = T_NONE;
    switch (node->getKind()) {
        case LiteralKind::Int : t = T_INTEGER; break;
        case LiteralKind::Real : t = T_REAL; break;
        case LiteralKind::Char : t = T_CHAR; break;
        case LiteralKind::String : t = T_STRING; break;
        case LiteralKind::Bool : t = T_BOOLEAN; break;
    }
    return t;
}