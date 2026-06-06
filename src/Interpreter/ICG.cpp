#include "ICG.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>

std::vector<Bytecode> IntermediateCode::generate(std::shared_ptr<ASTNode> root) {
    code.clear();
    procAddr.clear();
    currentLevel = 0;
    blockStack.clear();
    blockStack.push_back(0);
    currentFuncName = "";
    currentFuncTabIdx = -1;

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
    for (int lvl = currentLevel; lvl >= 0; --lvl) {
        if (lvl >= (int)blockStack.size()) continue;
        int blkIdx = blockStack[lvl];
        if (blkIdx < 0 || blkIdx >= symTab.getBlocktabSize()) continue;

        int idx = symTab.getBlockTab(blkIdx).last;
        while (idx > 0 && idx < symTab.getTabSize()) {
            if (symTab.getTab(idx).id == name)
                return idx;
            idx = symTab.getTab(idx).link;
        }
    }
    // Search predefined / global tab entries not caught by block chain
    for (int i = 0; i < symTab.getTabSize(); ++i) {
        if (symTab.getTab(i).id == name)
            return i;
    }
    return -1;
}

int IntermediateCode::levelDiff(int tabIdx) const {
    return currentLevel - symTab.getTab(tabIdx).lev;
}

int IntermediateCode::frameSizeFromBlock(int blockIdx) const {
    return 3 + symTab.getBlockTab(blockIdx).vsze;
}

OprCode IntermediateCode::opToOpr(const std::string& op) const {
    if (op == "plus"  || op == "+")   return OprCode::ADD;
    if (op == "minus" || op == "-")   return OprCode::SUB;
    if (op == "times" || op == "*")   return OprCode::MUL;
    if (op == "rdiv"  || op == "/")   return OprCode::DIV;
    if (op == "idiv"  || op == "div") return OprCode::DIV;
    if (op == "imod"  || op == "mod") return OprCode::MOD;
    if (op == "eql"   || op == "=")   return OprCode::EQL;
    if (op == "neq"   || op == "<>")  return OprCode::NEQ;
    if (op == "lss"   || op == "<")   return OprCode::LSS;
    if (op == "geq"   || op == ">=")  return OprCode::GEQ;
    if (op == "gtr"   || op == ">")   return OprCode::GTR;
    if (op == "leq"   || op == "<=")  return OprCode::LEQ;
    throw std::runtime_error("ICG: operator tidak dikenal: " + op);
}

void IntermediateCode::genProgram(std::shared_ptr<ProgramNode> node) {
    int mainBlockIdx = 0;
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

// Helper: find function/proc btab index by name and object type
int IntermediateCode::findProcBtab(const std::string& name, int objType) const {
    for (int i = 0; i < symTab.getTabSize(); ++i) {
        const Tab& t = symTab.getTab(i);
        if (t.id == name && t.obj == objType)
            return t.ref;
    }
    return -1;
}

void IntermediateCode::genProcDecl(std::shared_ptr<ProcDeclNode> node) {
    procAddr[node->getName()] = nextAddr();

    int blkIdx = findProcBtab(node->getName(), 5 /* OBJ_PROC */);

    currentLevel++;
    if ((int)blockStack.size() <= currentLevel)
        blockStack.push_back(blkIdx);
    else
        blockStack[currentLevel] = blkIdx;

    int fsize = (blkIdx >= 0) ? frameSizeFromBlock(blkIdx) : 3;
    emit(Opcode::INT, 0, fsize);

    std::string savedFunc = currentFuncName;
    int savedFuncTabIdx = currentFuncTabIdx;
    currentFuncName = "";
    currentFuncTabIdx = -1;

    if (node->getBody())
        genStmt(node->getBody());

    emit(Opcode::RET, 0, 0);

    currentFuncName = savedFunc;
    currentFuncTabIdx = savedFuncTabIdx;
    currentLevel--;
    blockStack.resize(currentLevel + 1);
}

void IntermediateCode::genFuncDecl(std::shared_ptr<FuncDeclNode> node) {
    procAddr[node->getName()] = nextAddr();

    int blkIdx = findProcBtab(node->getName(), 4 /* OBJ_FUNC */);

    int funcTabIdx = -1;
    for (int i = 0; i < symTab.getTabSize(); ++i) {
        if (symTab.getTab(i).id == node->getName() && symTab.getTab(i).obj == 4)
            funcTabIdx = i;
    }

    currentLevel++;
    if ((int)blockStack.size() <= currentLevel)
        blockStack.push_back(blkIdx);
    else
        blockStack[currentLevel] = blkIdx;

    std::string savedFunc = currentFuncName;
    int savedFuncTabIdx = currentFuncTabIdx;
    currentFuncName = node->getName();
    currentFuncTabIdx = funcTabIdx;

    int fsize = (blkIdx >= 0) ? frameSizeFromBlock(blkIdx) : 3;
    emit(Opcode::INT, 0, fsize);

    if (node->getBody())
        genStmt(node->getBody());

    emit(Opcode::RET, 0, 0);

    currentFuncName = savedFunc;
    currentFuncTabIdx = savedFuncTabIdx;
    currentLevel--;
    blockStack.resize(currentLevel + 1);
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
        case ASTType::FieldAccessNode:
            genFieldAccess(std::dynamic_pointer_cast<FieldAccessNode>(node)); break;
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
        case LiteralKind::String: {
            std::string raw = node->getValue();
            if (raw.size() >= 2 && raw.front() == '\'' && raw.back() == '\'')
                raw = raw.substr(1, raw.size() - 2);
            else if (raw.size() >= 1 && raw.front() == '\'')
                raw = raw.substr(1);
            int strIdx = internString(raw);
            emit(Opcode::LITS, 0, strIdx);
            break;
        }
    }
}

void IntermediateCode::genVarRef(std::shared_ptr<VarRefNode> node) {
    if (!node) return;

    if (node->getName() == "true") {
        emit(Opcode::LIT, 0, 1);
        return;
    }
    if (node->getName() == "false") {
        emit(Opcode::LIT, 0, 0);
        return;
    }

    int idx = lookupVar(node->getName());
    if (idx < 0) {
        std::cerr << "ICG: variabel tidak ditemukan: " << node->getName() << "\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }
    const Tab& entry = symTab.getTab(idx);

    if (entry.obj == 1) {
        // Constant
        int val = 0;
        if (!entry.const_value.empty()) {
            if (entry.const_value == "true")       val = 1;
            else if (entry.const_value == "false") val = 0;
            else {
                try { val = std::stoi(entry.const_value); }
                catch (...) { val = 0; }
            }
        }
        emit(Opcode::LIT, 0, val);
    } else {
        emit(Opcode::LOD, levelDiff(idx), varAddr(idx));
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

int IntermediateCode::getRecordFieldAddr(int recTabIdx, const std::string& fieldName) const {
    int blkRef = symTab.getTab(recTabIdx).ref;
    if (blkRef > 0 && blkRef < symTab.getBlocktabSize()) {
        int cur = symTab.getBlockTab(blkRef).last;
        while (cur > 0 && cur < symTab.getTabSize()) {
            if (symTab.getTab(cur).id == fieldName)
                return symTab.getTab(cur).adr;
            cur = symTab.getTab(cur).link;
        }
    }
    
    int recLev = symTab.getTab(recTabIdx).lev;
    for (int i = 0; i < symTab.getTabSize(); ++i) {
        if (symTab.getTab(i).id == fieldName && symTab.getTab(i).lev == recLev + 1)
            return symTab.getTab(i).adr;
    }
    return 0;
}

void IntermediateCode::genStore(std::shared_ptr<ExprNode> target) {
    if (!target) return;

    if (auto vr = std::dynamic_pointer_cast<VarRefNode>(target)) {
        std::string name = vr->getName();

        if (!currentFuncName.empty() && name == currentFuncName) {
            emit(Opcode::STO, 0, FUNC_RETVAL_SLOT);
            return;
        }

        int idx = lookupVar(name);
        if (idx < 0) {
            std::cerr << "ICG: variabel tidak ditemukan saat store: " << name << "\n";
            return;
        }
        emit(Opcode::STO, levelDiff(idx), varAddr(idx));

    } else if (auto fa = std::dynamic_pointer_cast<FieldAccessNode>(target)) {
        auto base = std::dynamic_pointer_cast<VarRefNode>(fa->getRecord());
        if (!base) {
            std::cerr << "ICG: record store kompleks belum didukung\n";
            return;
        }
        int recIdx = lookupVar(base->getName());
        if (recIdx < 0) {
            std::cerr << "ICG: record tidak ditemukan: " << base->getName() << "\n";
            return;
        }
        int fieldAddr = getRecordFieldAddr(recIdx, fa->getFieldName());
        emit(Opcode::STO, levelDiff(recIdx), varAddr(recIdx) + fieldAddr);

    } else if (auto arrAcc = std::dynamic_pointer_cast<ArrayAccessNode>(target)) {
        auto base = std::dynamic_pointer_cast<VarRefNode>(arrAcc->getArray());
        if (!base) {
            std::cerr << "ICG: array store kompleks belum didukung\n";
            return;
        }
        int idx = lookupVar(base->getName());
        if (idx < 0) {
            std::cerr << "ICG: array tidak ditemukan: " << base->getName() << "\n";
            return;
        }

        int low = 0;
        int aref = symTab.getTab(idx).ref;
        if (aref >= 0 && aref < symTab.getArraytabSize())
            low = symTab.getArrayTab(aref).low;

        int baseAddr = varAddr(idx);
        int lev = levelDiff(idx);

        emit(Opcode::LIT, 0, baseAddr);
        // Push index expr
        if (!!arrAcc->getIndex())
            genExpr(arrAcc->getIndex());
        else
            emit(Opcode::LIT, 0, 0);
        emit(Opcode::LIT, 0, low);
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::SUB));
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::ADD));
        emit(Opcode::STOI, lev, 0);
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
    int adr = varAddr(varIdx);
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
    emit(Opcode::LIT, 0, 0);
    emit(Opcode::OPR, 0, static_cast<int>(OprCode::EQL));  // NOT
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
        int nArgs = static_cast<int>(call->getArgs().size());
        for (auto& arg : call->getArgs())
            genExpr(arg);
        int index = lookupVar(name);
        (void)index;
        emit(Opcode::CAL, nArgs, it->second);
    }
}

void IntermediateCode::genBinaryOp(std::shared_ptr<BinaryOpNode> node) {
    if (!node) return;

    std::string op = node->getOp();

    if (op == "and" || op == "andsy") {
        genExpr(node->getLeft());
        int jpc1 = emit(Opcode::JPC, 0, 0);
        genExpr(node->getRight());
        int jpc2 = emit(Opcode::JPC, 0, 0);  
        emit(Opcode::LIT, 0, 1); 
        int jmpEnd = emit(Opcode::JMP, 0, 0);
        int falseLabel = nextAddr();
        emit(Opcode::LIT, 0, 0);
        int endLabel = nextAddr();
        patch(jpc1, falseLabel);
        patch(jpc2, falseLabel);
        patch(jmpEnd, endLabel);
        return;
    }

    if (op == "or" || op == "orsy") {
        // Short-circuit OR: if left is true, result is true
        genExpr(node->getLeft());
        int jpcLeft = emit(Opcode::JPC, 0, 0);
        emit(Opcode::LIT, 0, 1);
        int jmpEnd1 = emit(Opcode::JMP, 0, 0);
        int checkRight = nextAddr();
        genExpr(node->getRight());
        int jpcRight = emit(Opcode::JPC, 0, 0);
        emit(Opcode::LIT, 0, 1); 
        int jmpEnd2 = emit(Opcode::JMP, 0, 0);
        int falseLabel = nextAddr();
        emit(Opcode::LIT, 0, 0);   
        int endLabel = nextAddr();
        patch(jpcLeft, checkRight);
        patch(jmpEnd1, endLabel);
        patch(jpcRight, falseLabel);
        patch(jmpEnd2, endLabel);
        return;
    }

    genExpr(node->getLeft());
    genExpr(node->getRight());
    emit(Opcode::OPR, 0, static_cast<int>(opToOpr(op)));
}

void IntermediateCode::genUnaryOp(std::shared_ptr<UnaryOpNode> node) {
    if (!node) return;
    genExpr(node->getOperand());
    std::string op = node->getOp();
    if (op == "-" || op == "minus")
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::NEG));
    else if (op == "not" || op == "notsy") {
        emit(Opcode::LIT, 0, 0);
        emit(Opcode::OPR, 0, static_cast<int>(OprCode::EQL));
    }
}

void IntermediateCode::genCall(std::shared_ptr<CallNode> node) {
    if (!node) return;
    std::string name = node->getName();

    auto it = procAddr.find(name);
    if (it == procAddr.end()) {
        std::cerr << "ICG: fungsi '" << name << "' tidak ditemukan, push 0\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }

    // Push arguments before call
    int nArgs = static_cast<int>(node->getArgs().size());
    for (auto& arg : node->getArgs())
        genExpr(arg);

    int idx = lookupVar(name);
    int lev = (idx >= 0) ? levelDiff(idx) : 0;
    emit(Opcode::CAL, nArgs, it->second);
    // RETVAL pops the nArgs items that were pushed for arguments
    emit(Opcode::RETVAL, 0, nArgs);
}

void IntermediateCode::genArrayAccess(std::shared_ptr<ArrayAccessNode> node) {
    if (!node) return;
    auto base = std::dynamic_pointer_cast<VarRefNode>(node->getArray());
    if (!base) {
        std::cerr << "ICG: array access kompleks belum didukung\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }
    int idx = lookupVar(base->getName());
    if (idx < 0) {
        std::cerr << "ICG: array tidak ditemukan: " << base->getName() << "\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }

    // Get array metadata: low bound
    int low = 0;
    int aref = symTab.getTab(idx).ref;
    if (aref >= 0 && aref < symTab.getArraytabSize())
        low = symTab.getArrayTab(aref).low;

    int baseAddr = varAddr(idx);
    int lev = levelDiff(idx);

    // Push base address as literal
    emit(Opcode::LIT, 0, baseAddr);
    // Push index expression
    if (!!node->getIndex())
        genExpr(node->getIndex());
    else
        emit(Opcode::LIT, 0, 0);

    emit(Opcode::LIT, 0, low);
    emit(Opcode::OPR, 0, static_cast<int>(OprCode::SUB));
    emit(Opcode::OPR, 0, static_cast<int>(OprCode::ADD));
    emit(Opcode::LODI, lev, 0);
}

void IntermediateCode::genFieldAccess(std::shared_ptr<FieldAccessNode> node) {
    if (!node) return;
    auto base = std::dynamic_pointer_cast<VarRefNode>(node->getRecord());
    if (!base) {
        std::cerr << "ICG: field access kompleks belum didukung\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }
    int recIdx = lookupVar(base->getName());
    if (recIdx < 0) {
        std::cerr << "ICG: record tidak ditemukan: " << base->getName() << "\n";
        emit(Opcode::LIT, 0, 0);
        return;
    }
    int fieldAddr = getRecordFieldAddr(recIdx, node->getFieldName());
    emit(Opcode::LOD, levelDiff(recIdx), varAddr(recIdx) + fieldAddr);
}