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

int SemanticAnalyzer::visitVarRef(std::shared_ptr<VarRefNode> node) {
    if (!node) return T_NONE;
    std::string name = node->getName();
    int idx = symTab.searchTab(name);
    if (idx < 0) {
        semanticError("Undeclared identifier '" + node->getName() + "'");
        return T_NONE;
    }
    Tab& e = symTab.getTab(idx);
    if (e.obj == OBJ_TYPE) {
        semanticError("'" + node->getName() + "' is a type name, not a variable");
        return T_NONE;
    }
    return e.type;
}

int SemanticAnalyzer::visitCall(std::shared_ptr<CallNode> node) {
    if (!node) return T_NONE;
    std::string name = node->getName();
    int idx = symTab.searchTab(name);
    if (idx < 0) {
        semanticError("Undeclared procedure/function '" + node->getName() + "'");
        for (auto& arg : node->getArgs()) visitExpr(arg);
        return T_NONE;
    }

    Tab& e = symTab.getTab(idx);
    if (e.obj != OBJ_PROC && e.obj != OBJ_FUNC)
        semanticError("'" + node->getName() + "' is not a procedure or function");

    for (auto& arg : node->getArgs()) visitExpr(arg);

    return e.type; 
}

int SemanticAnalyzer::visitBinaryOp(std::shared_ptr<BinaryOpNode> node) {
    if (!node) return T_NONE;
    int lt = visitExpr(node->getLeft());
    int rt = visitExpr(node->getRight());
    const std::string& op = node->getOp();

    //relational operators
    if (op == "eql" || op == "neq" || op == "gtr" || op == "geq"
     || op == "lss" || op == "leq") {
        if (!isCompatible(lt, rt) && lt != T_NONE && rt != T_NONE)
            semanticError("Type mismatch in relational operator '" + op + "': "
                          + typeToString(lt) + " vs " + typeToString(rt));
        return T_BOOLEAN;
    }

    // bool operators
    if (op == "andsy" || op == "orsy") {
        if (lt != T_BOOLEAN && lt != T_NONE)
            semanticError("'" + op + "' requires Boolean left operand, got " + typeToString(lt));
        if (rt != T_BOOLEAN && rt != T_NONE)
            semanticError("'" + op + "' requires Boolean right operand, got " + typeToString(rt));
        return T_BOOLEAN;
    }

    // addition and multiplication
    if (op == "plus" || op == "minus" || op == "times") {
        bool lNum = (lt == T_INTEGER || lt == T_REAL);
        bool rNum = (rt == T_INTEGER || rt == T_REAL);
        if (lt != T_NONE && rt != T_NONE && (!lNum || !rNum))
            semanticError("Arithmetic operator '" + op + "' requires numeric operands, got "
                          + typeToString(lt) + " and " + typeToString(rt));
        if (lt == T_REAL || rt == T_REAL) return T_REAL;
        return T_INTEGER;
    }

    // real division
    if (op == "rdiv") {
        bool lNum = (lt == T_INTEGER || lt == T_REAL);
        bool rNum = (rt == T_INTEGER || rt == T_REAL);
        if (lt != T_NONE && !lNum)
            semanticError("'/' requires numeric left operand, got " + typeToString(lt));
        if (rt != T_NONE && !rNum)
            semanticError("'/' requires numeric right operand, got " + typeToString(rt));
        return T_REAL;
    }

    // division and modulo
    if (op == "idiv" || op == "imod") {
        if (lt != T_INTEGER && lt != T_NONE)
            semanticError("'" + op + "' requires Integer left operand, got " + typeToString(lt));
        if (rt != T_INTEGER && rt != T_NONE)
            semanticError("'" + op + "' requires Integer right operand, got " + typeToString(rt));
        return T_INTEGER;
    }

    semanticError("Unknown binary operator '" + op + "'");
    return T_NONE;
}

int SemanticAnalyzer::visitUnaryOp(std::shared_ptr<UnaryOpNode> node) {
    if (!node) return T_NONE;
    int t = visitExpr(node->getOperand());
    const std::string& op = node->getOp();

    if (op == "notsy") {
        if (t != T_BOOLEAN && t != T_NONE)
            semanticError("'not' requires Boolean operand, got " + typeToString(t));
        return T_BOOLEAN;
    }
    if (op == "plus" || op == "minus") {
        if (t != T_INTEGER && t != T_REAL && t != T_NONE)
            semanticError("Unary '" + op + "' requires numeric operand, got " + typeToString(t));
        return t;
    }
    return t;
}

int SemanticAnalyzer::visitArrayAccess(std::shared_ptr<ArrayAccessNode> node) {
    if (!node) return T_NONE;
    int arrType = visitExpr(node->getArray());
    int idxType = visitExpr(node->getIndex());

    if (arrType != T_ARRAY && arrType != T_NONE)
        semanticError("Cannot index into non-array type " + typeToString(arrType));
    if (idxType != T_INTEGER && idxType != T_CHAR && idxType != T_BOOLEAN && idxType != T_NONE)
        semanticError("Array index must be ordinal type, got " + typeToString(idxType));

    auto arr = node->getArray();
    while (arr && arr->getASTType() == ASTType::ArrayAccessNode)
        arr = std::dynamic_pointer_cast<ArrayAccessNode>(arr)->getArray();

    if (arr && arr->getASTType() == ASTType::VarRefNode) {
        auto vr = std::dynamic_pointer_cast<VarRefNode>(arr);
        int idx = symTab.searchTab(vr->getName());
        if (idx >= 0) {
            int ref = symTab.getTab(idx).ref;
            if (ref > 0 && ref < symTab.getArraytabSize())
                return symTab.getArrayTab(ref).etyp;
        }
    }
    return T_NONE;
}

int SemanticAnalyzer::visitFieldAccess(std::shared_ptr<FieldAccessNode> node) {
    if (!node) return T_NONE;
    int recType = visitExpr(node->getRecord());

    if (recType != T_RECORD && recType != T_NONE)
        semanticError("Field access on non-record type " + typeToString(recType));

    auto rec = node->getRecord();
    while (rec && (rec->getASTType() == ASTType::FieldAccessNode
                || rec->getASTType() == ASTType::ArrayAccessNode)) {
        if (rec->getASTType() == ASTType::FieldAccessNode)
            rec = std::dynamic_pointer_cast<FieldAccessNode>(rec)->getRecord();
        else
            rec = std::dynamic_pointer_cast<ArrayAccessNode>(rec)->getArray();
    }

    if (rec && rec->getASTType() == ASTType::VarRefNode) {
        auto vr = std::dynamic_pointer_cast<VarRefNode>(rec);
        int idx = symTab.searchTab(vr->getName());
        if (idx >= 0) {
            int ref = symTab.getTab(idx).ref;
            if (ref > 0 && ref < symTab.getBlocktabSize()) {
                // Search field in the record's block
                int fldIdx = symTab.getBlockTab(ref).last;
                std::string fld = node->getFieldName();
                while (fldIdx > 0) {
                    if (symTab.getTab(fldIdx).id == fld)
                        return symTab.getTab(fldIdx).type;
                    fldIdx = symTab.getTab(fldIdx).link;
                }
            }
        }
    }

    // Fallback: direct lookup
    int fldIdx = symTab.searchTab(node->getFieldName());
    if (fldIdx >= 0) return symTab.getTab(fldIdx).type;

    semanticError("Field '" + node->getFieldName() + "' not found in record");
    return T_NONE;
}

// ========================== Type Spec visitors ============================================

int SemanticAnalyzer::visitSimpleType(std::shared_ptr<SimpleTypeNode> node) {
    if (!node) return T_NONE;
    std::string name = node->getName();

    // Built-in types resolved directly (case-insensitive)
    if (name == "integer") return T_INTEGER;
    if (name == "real") return T_REAL;
    if (name == "boolean") return T_BOOLEAN;
    if (name == "char") return T_CHAR;
    if (name == "string")  return T_STRING;

    int idx = symTab.searchTab(name);
    if (idx < 0) {
        semanticError("Unknown type '" + node->getName() + "'");
        return T_NONE;
    }
    Tab& e = symTab.getTab(idx);
    if (e.obj != OBJ_TYPE) {
        semanticError("'" + node->getName() + "' is not a type identifier");
        return T_NONE;
    }
    lastTypeRef = e.ref;
    return e.type;
}

int SemanticAnalyzer::visitArrayType(std::shared_ptr<ArrayTypeNode> node) {
    if (!node) return T_ARRAY;

    // Resolve index type (must not be Real)
    lastTypeRef = 0;
    int idxType = visitType(node->getIndex());
    if (idxType == T_REAL)
        semanticError("Array index type cannot be Real");

    // Resolve element type
    lastTypeRef = 0;
    int elemType = visitType(node->getType());
    int elemRef  = lastTypeRef;

    // Extract bounds from a RangeTypeNode index
    int low = 0, high = 0;
    if (node->getIndex() && node->getIndex()->getKind() == TypeNode::Kind::Range) {
        auto rn = std::dynamic_pointer_cast<RangeTypeNode>(node->getIndex());
        if (rn) {
            if (auto ll = std::dynamic_pointer_cast<LiteralNode>(rn->getLow()))
                try { low  = std::stoi(ll->getValue()); } catch (...) {}
            if (auto hl = std::dynamic_pointer_cast<LiteralNode>(rn->getHigh()))
                try { high = std::stoi(hl->getValue()); } catch (...) {}
        }
    }

    int atIdx = symTab.addArray(idxType, elemType, elemRef, low, high);
    lastTypeRef = atIdx;
    return T_ARRAY;
}

int SemanticAnalyzer::visitEnumType(std::shared_ptr<EnumTypeNode> node) {
    if (!node) return T_INTEGER;
    int ordinal = 0;
    for (const auto& val : node->getEnumValues()) {
        std::string name = val;
        if (symTab.searchCurrentScope(name) >= 0) {
            semanticError("Duplicate enumeration value '" + val + "'");
        } else {
            int idx = symTab.addTab(name, OBJ_CONST, T_INTEGER, 0, 1, ordinal);
            symTab.getTab(idx).const_value = val;
            ordinal++;
        }
    }
    return T_INTEGER;
}

int SemanticAnalyzer::visitRangeType(std::shared_ptr<RangeTypeNode> node) {
    if (!node) return T_INTEGER;

    int lowType  = visitExpr(node->getLow());
    int highType = visitExpr(node->getHigh());

    if (lowType == T_REAL || highType == T_REAL)
        semanticError("Subrange type cannot use Real bounds");
    if (lowType != T_NONE && highType != T_NONE && lowType != highType)
        semanticError("Subrange bounds must have compatible types");

    // Validate low <= high for integer literals
    auto lowLit  = std::dynamic_pointer_cast<LiteralNode>(node->getLow());
    auto highLit = std::dynamic_pointer_cast<LiteralNode>(node->getHigh());
    if (lowLit && highLit
     && lowLit->getKind()  == LiteralKind::Int
     && highLit->getKind() == LiteralKind::Int) {
        try {
            int lo = std::stoi(lowLit->getValue());
            int hi = std::stoi(highLit->getValue());
            if (lo > hi)
                semanticError("Subrange lower bound (" + std::to_string(lo) + ") cannot exceed upper bound (" + std::to_string(hi) + ")");
        } catch (...) {}
    }

    // Record the range in atab 
    int lo = 0, hi = 0;
    if (lowLit)  try { lo = std::stoi(lowLit->getValue());  } catch (...) {}
    if (highLit) try { hi = std::stoi(highLit->getValue()); } catch (...) {}
    int atIdx = symTab.addArray(lowType, lowType, 0, lo, hi);
    lastTypeRef = atIdx;

    return (lowType != T_NONE) ? lowType : T_INTEGER;
}

int SemanticAnalyzer::visitFieldType(std::shared_ptr<FieldTypeNode> node) {
    if (!node) return T_RECORD;

    // Create a new block 
    symTab.enterScope();
    int blockIdx = symTab.getCurrentBlock();

    int offset = 0;
    for (auto& [names, typeSpec] : node->getFields()) {
        lastTypeRef = 0;
        int ft  = visitType(typeSpec);
        int ref = lastTypeRef;
        for (const auto& rawName : names) {
            std::string name = rawName;
            if (symTab.searchCurrentScope(name) >= 0) {
                semanticError("Duplicate field '" + rawName + "' in record");
            } else {
                symTab.addTab(name, OBJ_VAR, ft, ref, 1, offset++);
            }
        }
    }

    symTab.exitScope();
    lastTypeRef = blockIdx;
    return T_RECORD;
}

// ================================= HELPER FUNCTIONS ==================================

// Two types are compatible if they are the same, or one is a numeric widening of the other.
bool SemanticAnalyzer::isCompatible(int type1, int type2) const {
    if (type1 == T_NONE || type2 == T_NONE) return true; // already reported, don't cascade
    if (type1 == type2) return true;
    if ((type1 == T_INTEGER && type2 == T_REAL) || (type1 == T_REAL && type2 == T_INTEGER))
        return true;
    return false;
}

// T2 is assignment-compatible with T1 when:
//   (a) they are compatible
//   (b) T1 is Real and T2 is Integer (widening).
bool SemanticAnalyzer::isAssignmentCompatible(int t1, int t2) const {
    if (t1 == T_NONE || t2 == T_NONE) return true;
    if (t1 == t2) return true;
    if (t1 == T_REAL && t2 == T_INTEGER) return true;
    return isCompatible(t1, t2);
}

std::string SemanticAnalyzer::typeToString(int typeCode) const { 
    switch (typeCode) {
        case T_NONE:    return "none";
        case T_INTEGER: return "integer";
        case T_REAL:    return "real";
        case T_BOOLEAN: return "boolean";
        case T_CHAR:    return "char";
        case T_ARRAY:   return "array";
        case T_RECORD:  return "record";
        case T_STRING:  return "string";
        default:        return "unknown(" + std::to_string(typeCode) + ")";
    }
}

std::string SemanticAnalyzer::annotateExpr(std::shared_ptr<ExprNode> node) const {
    if (!node) return "";
    int t = getCachedType(node.get());
    std::string ann = " \u2192 type:" + typeToString(t); // "→ type:…"

    if (node->getASTType() == ASTType::VarRefNode) {
        auto vr = std::dynamic_pointer_cast<VarRefNode>(node);
        int idx = symTab.searchTab(vr->getName());  // const-access via mutable sym
        if (idx >= 0)
            ann += ", tab_idx:" + std::to_string(idx)
                 + ", lev:" + std::to_string(symTab.getTab(idx).lev);
    }
    return ann;
}