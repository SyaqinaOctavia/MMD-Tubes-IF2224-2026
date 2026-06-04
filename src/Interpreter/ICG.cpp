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

void IntermediateCode::genStore(std::shared_ptr<ExprNode> target) {
    if (!target) return;

    if (auto vr = std::dynamic_pointer_cast<VarRefNode>(target)) {
        int idx = lookupVar(vr->getName());
        if (idx < 0) {
            std::cerr << "ICG: variabel tidak ditemukan saat store: " << vr->getName() << "\n";
            return;
        }
        emit(Opcode::STO, levelDiff(idx), symTab.getTab(idx).adr);

    } else if (auto aa = std::dynamic_pointer_cast<ArrayAccessNode>(target)) {
        auto base = std::dynamic_pointer_cast<VarRefNode>(aa->getArray());
        if (!base) { std::cerr << "ICG: array store kompleks belum didukung\n"; return; }
        int idx = lookupVar(base->getName());
        if (idx < 0) { std::cerr << "ICG: array tidak ditemukan: " << base->getName() << "\n"; return; }
        genExpr(aa->getIndex());
        emit(Opcode::STO, levelDiff(idx), symTab.getTab(idx).adr);
    }
}

void IntermediateCode::genIf(std::shared_ptr<IfNode> node) {
    if (!node) return;
    genExpr(node->getCondition());
    int jpcAddr = emit(Opcode::JPC, 0, 0);

    genStmt(node->getThenBlock());

    if (node->getElseBlock()) {
        int jmpAddr = emit(Opcode::JMP, 0, 0);
        patch(jpcAddr, nextAddr());
        genStmt(node->getElseBlock());
        patch(jmpAddr, nextAddr());
    } else {
        patch(jpcAddr, nextAddr());
    }
}

void IntermediateCode::genWhile(std::shared_ptr<WhileNode> node) {
    if (!node) return;
    int loopStart = nextAddr();
    genExpr(node->getCondition());
    int jpcAddr = emit(Opcode::JPC, 0, 0);
    genStmt(node->getBody());
    emit(Opcode::JMP, 0, loopStart);
    patch(jpcAddr, nextAddr());
}

void IntermediateCode::genFor(std::shared_ptr<ForNode> node) {
    if (!node) return;

    int varIdx = lookupVar(node->getMovingVar());
    if (varIdx < 0) {
        std::cerr << "ICG: FOR variable tidak ditemukan: " << node->getMovingVar() << "\n";
        return;
    }
    int lev = levelDiff(varIdx);
    int addr = symTab.getTab(varIdx).adr;

    genExpr(node->getStartPoint());
    emit(Opcode::STO, lev, addr);
    int loopStart = nextAddr();
    emit(Opcode::LOD, lev, addr);
    genExpr(node->getEndPoint());

    if (node->goesUp())
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::LEQ));
    else
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::GEQ));

    int jpcAddr = emit(Opcode::JPC, 0, 0);
    genStmt(node->getBody());

    emit(Opcode::LOD, lev, addr);
    emit(Opcode::LIT, 0, 1);

    if (node->goesUp())
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::ADD));
    else
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::SUB));
    emit(Opcode::STO, lev, addr);

    emit(Opcode::JMP, 0, loopStart);
    patch(jpcAddr, nextAddr());
}

void IntermediateCode::genRepeat(std::shared_ptr<RepeatNode> node) {
    if (!node) return;
    int loopStart = nextAddr();
    genStmt(node->getBody());
    genExpr(node->getUntilCondition());
    emit(Opcode::JPC, 0, loopStart);
}

void IntermediateCode::genCase(std::shared_ptr<CaseNode> node) {
    if (!node) return;

    auto cases = node->getCases();
    std::vector<int> jmpToEndAddrs; 

    for (size_t i = 0; i < cases.size(); i++) {
        auto& [labels, stmt] = cases[i];

        std::vector<int> jpcToNext; 

        for (auto& lbl : labels) {
            genExpr(node->getKey());
            genExpr(lbl);
            emit(Opcode::OPR, 0, static_cast<int>(OprCode::EQL));
            jpcToNext.push_back(emit(Opcode::JPC, 0, 0));
        }

        if (!jpcToNext.empty()) {
            genStmt(stmt);
            int jmpEnd = emit(Opcode::JMP, 0, 0);
            jmpToEndAddrs.push_back(jmpEnd);
            for (int jpcAddr : jpcToNext)
                patch(jpcAddr, nextAddr());
        }
    }

    for (int jmpAddr : jmpToEndAddrs)
        patch(jmpAddr, nextAddr());
}


void IntermediateCode::genCallStmt(std::shared_ptr<CallStmtNode> node) {
    if (!node || !node->getCall()) return;

    auto call = node->getCall();
    std::string name = call->getName();

    if (name == "writeln") {
        auto args = call->getArgs();
        if (args.empty()) {
            emit(Opcode::LIT, 0, 0);
            emit(Opcode::OPR, 0, static_cast<int>(OprCode::WRTLN));
        } else {
            for (size_t i = 0; i < args.size(); i++) {
                genExpr(args[i]);
                if (i + 1 == args.size())
                    emit(Opcode::OPR, 0, static_cast<int>(OprCode::WRTLN));
                else
                    emit(Opcode::OPR, 0, static_cast<int>(OprCode::WRT));
            }
        }
    } else if (name == "write") {
        for (auto& arg : call->getArgs()) {
            genExpr(arg);
            emit(Opcode::OPR, 0, static_cast<int>(OprCode::WRT));
        }
    } else if (name == "readln") {
        std::cerr << "ICG: readln belum didukung penuh\n";
    } else {
        auto it = procAddr.find(name);
        if (it == procAddr.end()) {
            std::cerr << "ICG: prosedur tidak ditemukan: " << name << "\n";
            return;
        }
        for (auto& arg : call->getArgs())
            genExpr(arg);
        int index = lookupVar(name);
        int level = (index >= 0) ? levelDiff(index) : 0;
        emit(Opcode::CAL, level, it->second);
    }
}

void IntermediateCode::genStore(std::shared_ptr<ExprNode> target) {
    if (!target) return;

    if (auto vr = std::dynamic_pointer_cast<VarRefNode>(target)) {
        int idx = lookupVar(vr->getName());
        if (idx < 0) {
            std::cerr << "ICG: variabel tidak ditemukan saat store: " << vr->getName() << "\n";
            return;
        }
        emit(Opcode::STO, levelDiff(idx), symTab.getTab(idx).adr);

    } else if (auto aa = std::dynamic_pointer_cast<ArrayAccessNode>(target)) {
        auto base = std::dynamic_pointer_cast<VarRefNode>(aa->getArray());
        if (!base) { std::cerr << "ICG: array store kompleks belum didukung\n"; return; }
        int idx = lookupVar(base->getName());
        if (idx < 0) { std::cerr << "ICG: array tidak ditemukan: " << base->getName() << "\n"; return; }
        genExpr(aa->getIndex());
        emit(Opcode::STO, levelDiff(idx), symTab.getTab(idx).adr);
    }
}

void IntermediateCode::genIf(std::shared_ptr<IfNode> node) {
    if (!node) return;

    genExpr(node->getCondition());
    int jpcAddr = emit(Opcode::JPC, 0, 0);
    genStmt(node->getThenBlock());

    if (node->getElseBlock()) {
        int jmpAddr = emit(Opcode::JMP, 0, 0);
        patch(jpcAddr, nextAddr());
        genStmt(node->getElseBlock());
        patch(jmpAddr, nextAddr());
    } else {
        patch(jpcAddr, nextAddr());
    }
}

void IntermediateCode::genCase(std::shared_ptr<CaseNode> node) {
    if (!node) return;

    auto cases = node->getCases();
    std::vector<int> jmpToEndAddrs;

    for (size_t i = 0; i < cases.size(); i++) {
        auto& [labels, stmt] = cases[i];

        std::vector<int> jpcToNext;

        for (auto& lbl : labels) {
            genExpr(node->getKey());
            genExpr(lbl);
            emit(Opcode::OPR, 0, static_cast<int>(OprCode::EQL));
            jpcToNext.push_back(emit(Opcode::JPC, 0, 0));
        }

        if (!jpcToNext.empty()) {

            genStmt(stmt);
            int jmpEnd = emit(Opcode::JMP, 0, 0);
            jmpToEndAddrs.push_back(jmpEnd);

            for (int jpcAddr : jpcToNext)
                patch(jpcAddr, nextAddr());
        }
    }

    for (int jmpAddr : jmpToEndAddrs)
        patch(jmpAddr, nextAddr());
}

void IntermediateCode::genWhile(std::shared_ptr<WhileNode> node) {
    if (!node) return;

    int loopStart = nextAddr(); // catat posisi awal loop
    genExpr(node->getCondition());

    int jpcAddr = emit(Opcode::JPC, 0, 0);
    genStmt(node->getBody());
    emit(Opcode::JMP, 0, loopStart);
    patch(jpcAddr, nextAddr());
}

void IntermediateCode::genFor(std::shared_ptr<ForNode> node) {
    if (!node) return;

    int varIdx = lookupVar(node->getMovingVar());
    if (varIdx < 0) {
        std::cerr << "ICG: FOR variable tidak ditemukan: " << node->getMovingVar() << "\n";
        return;
    }
    int lev = levelDiff(varIdx);
    int adr = symTab.getTab(varIdx).adr;

    genExpr(node->getStartPoint());
    emit(Opcode::STO, lev, adr);


    int loopStart = nextAddr();

    emit(Opcode::LOD, lev, adr);
    genExpr(node->getEndPoint());

    if (node->goesUp())
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::LEQ));
    else
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::GEQ));

    int jpcAddr = emit(Opcode::JPC, 0, 0);

    genStmt(node->getBody());

    emit(Opcode::LOD, lev, adr);
    emit(Opcode::LIT, 0, 1);
    if (node->goesUp())
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::ADD));
    else
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::SUB));
    emit(Opcode::STO, lev, adr);

    emit(Opcode::JMP, 0, loopStart);
    patch(jpcAddr, nextAddr());
}

void IntermediateCode::genRepeat(std::shared_ptr<RepeatNode> node) {
    if (!node) return;

    int loopStart = nextAddr();
    genStmt(node->getBody());
    genExpr(node->getUntilCondition());
    emit(Opcode::JPC, 0, loopStart);
}