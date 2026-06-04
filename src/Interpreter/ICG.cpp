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

void IntermediateCode::genExpr(std::shared_ptr<ExprNode> node) {
    if (!node) return;
    switch (node->getASTType()) {
        case ASTType::LiteralNode:
            genLiteral(std::dynamic_pointer_cast<LiteralNode>(node)); break;
        case ASTType::VarRefNode:
            genVarRef(std::dynamic_pointer_cast<VarRefNode>(node)); break;
        case ASTType::BinaryOpNode:
            genBinaryOp(std::dynamic_pointer_cast<BinaryOpNode>(node)); break;
        case ASTType::UnaryOpNode:
            genUnaryOp(std::dynamic_pointer_cast<UnaryOpNode>(node)); break;
        case ASTType::CallNode:
            genCall(std::dynamic_pointer_cast<CallNode>(node)); break;
        case ASTType::ArrayAccessNode:
            genArrayAccess(std::dynamic_pointer_cast<ArrayAccessNode>(node)); break;
        default:
            std::cerr << "ICG: genExpr - tipe ekspresi tidak dikenal\n"; break;
    }
}

void IntermediateCode::genLiteral(std::shared_ptr<LiteralNode> node) {
    if (!node) return;
    int val = 0;
    switch (node->getKind()) {
        case LiteralKind::Int:
            val = std::stoi(node->getValue());
            emit(Opcode::LIT, 0, val);
            break;
        case LiteralKind::Bool:
            val = (node->getValue() == "true") ? 1 : 0;
            emit(Opcode::LIT, 0, val);
            break;
        case LiteralKind::Char:
            if (!node->getValue().empty())
                val = static_cast<int>(node->getValue()[0]);
            emit(Opcode::LIT, 0, val);
            break;
        case LiteralKind::Real:
            val = static_cast<int>(std::stod(node->getValue()));
            emit(Opcode::LIT, 0, val);
            break;
        case LiteralKind::String:
            std::cerr << "ICG: literal string '" << node->getValue()
                      << "' — push sebagai 0 (belum didukung penuh)\n";
            emit(Opcode::LIT, 0, 0);
            break;
    }
}

void IntermediateCode::genVarRef(std::shared_ptr<VarRefNode> node) {
    if (!node) return;
    int idx = lookupVar(node->getName());
    if (idx < 0) {
        std::cerr << "ICG: variabel tidak ditemukan: " << node->getName() << "\n";
        emit(Opcode::LIT, 0, 0); 
        return;
    }
    const Tab& entry = symTab.getTab(idx);

    if (entry.obj == 1) {
        int val = 0;
        if (!entry.const_value.empty()) {
            if (entry.const_value == "true") val = 1;
            else if (entry.const_value == "false") val = 0;
            else {
                try { val = std::stoi(entry.const_value); }
                catch (const std::exception& e) { val = 0; }
            }
        }
        emit(Opcode::LIT, 0, val);
    } else {
        emit(Opcode::LOD, levelDiff(idx), entry.adr);
    }
}