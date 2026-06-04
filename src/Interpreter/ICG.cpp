#include "ICG.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

std::vector<Bytecode> IntermediateCode::generate(std::shared_ptr<ASTNode> root) {
    code.clear();
    procAddr.clear();
    currentLevel = 0;

    if (!root) throw std::runtime_error("ICG: null AST root");

    auto prog = std::dynamic_pointer_cast<ProgramNode>(root);
    if (!prog) throw std::runtime_error("ICG: root bukan ProgramNode");

    genProgram(prog);
    return code;
}

void IntermediateCode::writeToFile(
    const std::vector<Bytecode>& instrs, const std::string& filename) const
{
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("ICG: gagal membuka file output: " + filename);
    writeToStream(instrs, out);
}

void IntermediateCode::writeToStream(
    const std::vector<Bytecode>& instrs, std::ostream& out) const
{
    for (int i = 0; i < static_cast<int>(instrs.size()); ++i)
        out << instrs[i].toString(i) << "\n";
}

int IntermediateCode::lookupVar(const std::string& name) const {
    int idx = const_cast<SymbolTable&>(symTab).searchTab(name);
    return idx;
}

int IntermediateCode::levelDiff(int tabIdx) const {
    return currentLevel - symTab.getTab(tabIdx).lev;
}

int IntermediateCode::frameSizeFromBlock(int blockIdx) const {
    return 3 + symTab.getBlockTab(blockIdx).vsze;
}

OprCode IntermediateCode::opToOpr(const std::string& op) const {
    if (op == "+")   return OprCode::ADD;
    if (op == "-")   return OprCode::SUB;
    if (op == "*")   return OprCode::MUL;
    if (op == "/")   return OprCode::DIV;
    if (op == "div") return OprCode::DIV;
    if (op == "mod") return OprCode::MOD;
    if (op == "=")   return OprCode::EQL;
    if (op == "<>")  return OprCode::NEQ;
    if (op == "<")   return OprCode::LSS;
    if (op == ">=")  return OprCode::GEQ;
    if (op == ">")   return OprCode::GTR;
    if (op == "<=")  return OprCode::LEQ;
    throw std::runtime_error("ICG: operator tidak dikenal: " + op);
}

void IntermediateCode::genProgram(std::shared_ptr<ProgramNode> node) {
    int mainBlockIdx = 1; 
    int fsize = 3; 

    if (symTab.getBlocktabSize() > mainBlockIdx) {
        fsize = frameSizeFromBlock(mainBlockIdx);
    }

    std::vector<std::shared_ptr<ProcDeclNode>> procs;
    std::vector<std::shared_ptr<FuncDeclNode>> funcs;
    for (auto& decl : node->getDeclarations()) {
        if (auto pd = std::dynamic_pointer_cast<ProcDeclNode>(decl))
            procs.push_back(pd);
        else if (auto fd = std::dynamic_pointer_cast<FuncDeclNode>(decl))
            funcs.push_back(fd);
    }

    int jmpToMain = -1;
    if (!procs.empty() || !funcs.empty())
        jmpToMain = emit(Opcode::JMP, 0, 0);

    for (auto& pd : procs) genProcDecl(pd);
    for (auto& fd : funcs) genFuncDecl(fd);

    if (jmpToMain >= 0)
        patch(jmpToMain, nextAddr());

    emit(Opcode::INT, 0, fsize);

    if (node->getMain())
        genCompound(node->getMain());

    emit(Opcode::RET, 0, 0);
}

void IntermediateCode::genStmt(std::shared_ptr<StmtNode> node) {
    if (!node) return;
    switch (node->getASTType()) {
        case ASTType::CompoundNode:
            genCompound(std::dynamic_pointer_cast<CompoundNode>(node)); break;
        case ASTType::AssignNode:
            genAssign(std::dynamic_pointer_cast<AssignNode>(node)); break;
        case ASTType::IfNode:
            genIf(std::dynamic_pointer_cast<IfNode>(node)); break;
        case ASTType::WhileNode:
            genWhile(std::dynamic_pointer_cast<WhileNode>(node)); break;
        case ASTType::ForNode:
            genFor(std::dynamic_pointer_cast<ForNode>(node)); break;
        case ASTType::RepeatNode:
            genRepeat(std::dynamic_pointer_cast<RepeatNode>(node)); break;
        case ASTType::CaseNode:
            genCase(std::dynamic_pointer_cast<CaseNode>(node)); break;
        case ASTType::CallStmtNode:
            genCallStmt(std::dynamic_pointer_cast<CallStmtNode>(node)); break;
        default:
            std::cerr << "ICG: genStmt - tipe statement tidak dikenal\n"; break;
    }
}

void IntermediateCode::genCompound(std::shared_ptr<CompoundNode> node) {
    if (!node) return;
    for (auto& stmt : node->getStatements())
        genStmt(stmt);
}

void IntermediateCode::genAssign(std::shared_ptr<AssignNode> node) {
    if (!node) return;
    genExpr(node->getValue());
    genStore(node->getTarget());
}

