#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "AST.hpp"
#include "SymbolTable.hpp"

class SemanticAnalyzer {
private:
    SymbolTable& symTab;
    int errorCount;

    const int T_NONE = 0;
    const int T_INTEGER = 1;
    const int T_REAL = 2;
    const int T_BOOLEAN = 3;
    const int T_CHAR = 4;
    const int T_ARRAY = 5;
    const int T_RECORD = 6;
    const int T_STRING = 7;

    const int OBJ_CONST = 1;
    const int OBJ_VAR = 2;
    const int OBJ_TYPE = 3;
    const int OBJ_FUNC = 4;
    const int OBJ_PROC = 5;

    void semanticError(const std::string& message) {
        std::cerr << "Semantic Error: " << message << std::endl;
        errorCount++;
    }

public:
    SemanticAnalyzer(SymbolTable& st) : symTab(st), errorCount(0) {}
    bool analyze(std::shared_ptr<ASTNode> root) {
        errorCount = 0;
        if (!root) return false;
        
        try {
            visit(root);
        } catch (const std::exception& e) {
            semanticError("Fatal crash saat semantic analysis: " + std::string(e.what()));
        }
        
        return errorCount == 0;
    }

    int getErrorCount() const { return errorCount; }

private:
    // main visitors
    void visit(std::shared_ptr<ASTNode> node);
    int visitExpr(std::shared_ptr<ExprNode> node);
    int visitType(std::shared_ptr<TypeNode> node);

    void visitProgram(std::shared_ptr<ProgramNode> node);
    void visitConstDecl(std::shared_ptr<ConstDeclNode> node);
    void visitTypeDecl(std::shared_ptr<TypeDeclNode> node);
    void visitVarDecl(std::shared_ptr<VarDeclNode> node);
    void visitParamDecl(std::shared_ptr<ParamDeclNode> node);
    void visitProcDecl(std::shared_ptr<ProcDeclNode> node);
    void visitFuncDecl(std::shared_ptr<FuncDeclNode> node);

    // statement visitors
    void visitAssign(std::shared_ptr<AssignNode> node);
    void visitIf(std::shared_ptr<IfNode> node);
    void visitWhile(std::shared_ptr<WhileNode> node);
    void visitFor(std::shared_ptr<ForNode> node);
    void visitRepeat(std::shared_ptr<RepeatNode> node);
    void visitCase(std::shared_ptr<CaseNode> node);
    void visitCompound(std::shared_ptr<CompoundNode> node);
    void visitCallStmt(std::shared_ptr<CallStmtNode> node);

    // expression visitors
    int visitLiteral(std::shared_ptr<LiteralNode> node);
    int visitCall(std::shared_ptr<CallNode> node);
    int visitBinaryOp(std::shared_ptr<BinaryOpNode> node);
    int visitUnaryOp(std::shared_ptr<UnaryOpNode> node);
    int visitVarRef(std::shared_ptr<VarRefNode> node);
    int visitArrayAccess(std::shared_ptr<ArrayAccessNode> node);
    int visitFieldAccess(std::shared_ptr<FieldAccessNode> node);

    // type specification visitors
    int visitSimpleType(std::shared_ptr<SimpleTypeNode> node);
    int visitArrayType(std::shared_ptr<ArrayTypeNode> node);
    int visitEnumType(std::shared_ptr<EnumTypeNode> node);
    int visitRangeType(std::shared_ptr<RangeTypeNode> node);
    int visitFieldType(std::shared_ptr<FieldTypeNode> node);

    // helpers
    bool isCompatible(int type1, int type2);
    bool isAssignmentCompatible(int targetType, int valueType);
    std::string typeToString(int typeCode);
};