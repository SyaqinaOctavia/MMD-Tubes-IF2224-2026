#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "Instructions.hpp"
#include "../Semantic/SymbolTable.hpp"
#include "../Semantic/AST.hpp"

static constexpr int FUNC_RETVAL_SLOT = 3;

class IntermediateCode {
private:
    SymbolTable& symTab;
    std::vector<Bytecode> code;
    int currentLevel;

    std::vector<int> blockStack;

    std::string currentFuncName;
    int currentFuncTabIdx;

    int emit(Opcode op, int level, int target) {
        int index = code.size();
        Bytecode newLine(index, op, level, target);
        code.push_back(newLine);
        return index;
    }

    void patch(int index, int newTarget) {
        if (index < 0 || index >= static_cast<int>(code.size()))
            throw std::runtime_error("ICG: patch ke alamat tidak valid: " + std::to_string(index));
        code.at(index).setTarget(newTarget);
    }

    int size() { return code.size(); }
    Bytecode& get(int index) { return code[index]; }
    int nextAddr() const { return static_cast<int>(code.size()); }

    void genProgram(std::shared_ptr<ProgramNode>  node);
    void genProcDecl(std::shared_ptr<ProcDeclNode> node);
    void genFuncDecl(std::shared_ptr<FuncDeclNode> node);

    void genStmt(std::shared_ptr<StmtNode> node);
    void genCompound(std::shared_ptr<CompoundNode> node);
    void genAssign(std::shared_ptr<AssignNode> node);
    void genIf(std::shared_ptr<IfNode> node);
    void genWhile(std::shared_ptr<WhileNode> node);
    void genFor(std::shared_ptr<ForNode> node);
    void genRepeat(std::shared_ptr<RepeatNode> node);
    void genCase(std::shared_ptr<CaseNode> node);
    void genCallStmt(std::shared_ptr<CallStmtNode> node);

    void genExpr(std::shared_ptr<ExprNode> node);
    void genLiteral(std::shared_ptr<LiteralNode> node);
    void genVarRef(std::shared_ptr<VarRefNode> node);
    void genBinaryOp(std::shared_ptr<BinaryOpNode> node);
    void genUnaryOp(std::shared_ptr<UnaryOpNode> node);
    void genCall(std::shared_ptr<CallNode> node);
    void genArrayAccess(std::shared_ptr<ArrayAccessNode> node);
    void genFieldAccess(std::shared_ptr<FieldAccessNode> node);
    void genStore(std::shared_ptr<ExprNode> target);

    int lookupVar(const std::string& name) const;
    int levelDiff(int tabIdx) const;

    int frameSize(std::shared_ptr<ProcDeclNode> node) const;
    int frameSize(std::shared_ptr<FuncDeclNode> node) const;
    int frameSizeFromBlock(int blockIdx) const;
    int findProcBtab(const std::string& name, int objType) const;
    int getRecordFieldAddr(int recTabIdx, const std::string& fieldName) const;
    int getArrayElementSlotCount(int arrayRef) const;
    bool collectArrayAccessInfo(std::shared_ptr<ArrayAccessNode> node,
                                int& baseIdx,
                                std::vector<std::shared_ptr<ExprNode>>& indices,
                                std::vector<int>& lowers,
                                std::vector<int>& multipliers,
                                int& fieldOffset) const;

    std::unordered_map<std::string, int> procAddr;

    int varAddr(int tabIdx) const { return symTab.getTab(tabIdx).adr + 3; }
    OprCode opToOpr(const std::string& op) const;

    std::vector<std::string> stringTable;
    int internString(const std::string& s) {
        for (int i = 0; i < (int)stringTable.size(); i++)
            if (stringTable[i] == s) return i;
        stringTable.push_back(s);
        return (int)stringTable.size() - 1;
    }

    std::vector<double> realValues;
    int internReal(double r) {
        for (int i = 0; i < (int)realValues.size(); i++)
            if (realValues[i] == r) return i;
        realValues.push_back(r);
        return (int)realValues.size() - 1;
    }

public:
    IntermediateCode(SymbolTable& symTab)
        : symTab(symTab), currentLevel(0), currentFuncTabIdx(-1) {}

    std::vector<Bytecode> generate(std::shared_ptr<ASTNode> root);

    void writeToFile(const std::vector<Bytecode>& instrs, const std::string& filename) const;
    void writeToStream(const std::vector<Bytecode>& instrs, std::ostream& out) const;
    const std::vector<std::string>& getStringTable() const { return stringTable; }
    const std::vector<double>& getRealValues() const { return realValues; }
};