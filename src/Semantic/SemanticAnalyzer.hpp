#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "AST.hpp"
#include "SymbolTable.hpp"

class SemanticAnalyzer {
private:
    SymbolTable& symTab;
    int errorCount;
    int lastTypeRef;

    static const int T_NONE = 0;
    static const int T_INTEGER = 1;
    static const int T_REAL = 2;
    static const int T_BOOLEAN = 3;
    static const int T_CHAR = 4;
    static const int T_ARRAY = 5;
    static const int T_RECORD = 6;
    static const int T_STRING = 7;

    static const int OBJ_CONST = 1;
    static const int OBJ_VAR = 2;
    static const int OBJ_TYPE = 3;
    static const int OBJ_FUNC = 4;
    static const int OBJ_PROC = 5;

    void semanticError(const std::string& message) {
        std::cerr << "Semantic Error: " << message << std::endl;
        errorCount++;
    }

    std::unordered_map<void*, int> nodeType;
    const std::string BRANCH = "\u251C\u2500 ";   // ├─
    const std::string CORNER = "\u2514\u2500 ";   // └─
    const std::string PIPE   = "\u2502  ";        // │
    const std::string SPACE  = "   ";
    std::string conn(bool isLast)  const {
        if(isLast) return CORNER;
        return BRANCH;
    }
    std::string ext (bool isLast)  const {
        if(isLast) return SPACE;
        return PIPE;  
    }

public:
    SemanticAnalyzer(SymbolTable& st) : lastTypeRef(0), symTab(st), errorCount(0) {}
    bool analyzeAndOutput(std::shared_ptr<ASTNode> root, std::string filename) {
        errorCount = 0;
        if (!root) return false;
        
        try {
            visit(root);
            std::ofstream output(filename);
            if(!output){
                std::cerr << "Failed to open output file: " << filename << std::endl;
                return false;
            }
            symTab.outputTable(output);
            printDecoratedAST(root, output);
        } catch (const std::exception& e) {
            semanticError("Fatal crash saat semantic analysis: " + std::string(e.what()));
        }
        
        return errorCount == 0;
    }

    int getErrorCount() const { return errorCount; }
    int getCachedType(void* ptr) const {
        auto it = nodeType.find(ptr);
        return it != nodeType.end() ? it->second : T_NONE;
    }
    void cacheType(void* ptr, int t) { nodeType[ptr] = t; }

private:
    int typeSlotSize(int t, int ref) const;

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
    bool isCompatible(int type1, int type2) const;
    bool isAssignmentCompatible(int targetType, int valueType) const;
    std::string typeToString(int typeCode) const;
    std::string annotateExpr(std::shared_ptr<ExprNode> node) const;
    void printExpr(std::shared_ptr<ExprNode> node, std::ostream& out, const std::string& prefix, bool isLast) const;
    void printStmt(std::shared_ptr<StmtNode> node, std::ostream& out, const std::string& prefix, bool isLast) const;
    void printNode(std::shared_ptr<ASTNode> node, std::ostream& out, const std::string& prefix, bool isLast) const;
    void printDecoratedAST(std::shared_ptr<ASTNode> root, std::ostream& out) const;
};