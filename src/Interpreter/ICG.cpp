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

