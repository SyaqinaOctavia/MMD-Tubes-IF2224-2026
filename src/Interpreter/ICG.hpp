#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "Instructions.hpp"
#include "../Semantic/SymbolTable.hpp"
#include "../Semantic/AST.hpp"

class IntermediateCode {
private:
    SymbolTable& symTab;
    std::vector<Bytecode> code;   // hasil instruksi yang sedang dibangun
    int currentLevel;                // lexical level saat ini (0 = global)

    int emit(Opcode op, int level, int target){
        int index = code.size();
        Bytecode newLine(index, op, level, target);
        code.push_back(newLine);
        return index;
    }
    
    void patch(int index, int newTarget){
        if (index < 0 || index >= static_cast<int>(code.size()))
        throw std::runtime_error("ICG: patch ke alamat tidak valid: " + std::to_string(index));
        code.at(index).setTarget(newTarget);
    }

    int size(){ return code.size(); }
    Bytecode& get(int index){ return code[index]; }

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
    void genStore     (std::shared_ptr<ExprNode>         target);

    int lookupVar(const std::string& name) const;

    int levelDiff(int tabIdx) const;

    int frameSize(std::shared_ptr<ProcDeclNode> node) const;
    int frameSize(std::shared_ptr<FuncDeclNode> node) const;
    int frameSizeFromBlock(int blockIdx) const;

    std::unordered_map<std::string, int> procAddr;
    int varAddr(int tabIdx) const { return symTab.getTab(tabIdx).adr + 3; }
    OprCode opToOpr(const std::string& op) const;
    
public:
    IntermediateCode(SymbolTable& symTab)
        : symTab(symTab), currentLevel(0) {}

    std::vector<Bytecode> generate(std::shared_ptr<ASTNode> root);

    void writeToFile(const std::vector<Bytecode>& instrs, const std::string& filename) const;

    void writeToStream(const std::vector<Bytecode>& instrs, std::ostream& out) const;

};