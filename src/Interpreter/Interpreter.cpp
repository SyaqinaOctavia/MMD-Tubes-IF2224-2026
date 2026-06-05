#include "Interpreter.hpp"
#include <sstream>

void Interpreter::execute(std::vector<Bytecode>& code) {
    execute(code, std::cout);
}

void Interpreter::execute(std::vector<Bytecode>& code, std::ostream& out) {
    if (code.empty()) return;
    std::fill(stack.begin(), stack.end(), 0);
    sp = -1;
    bp = 0;
    pc = 0;
    callDepth = 0;
    int codeSize = static_cast<int>(code.size());
    while (pc >= 0 && pc < codeSize) {
        Bytecode& instr = code[pc];
        pc++;

        switch (instr.getOp()) {
            case Opcode::LIT: execLIT(instr);             break;
            case Opcode::LOD: execLOD(instr);             break;
            case Opcode::STO: execSTO(instr);             break;
            case Opcode::CAL: execCAL(instr, code);       break;
            case Opcode::INT: execINT(instr);             break;
            case Opcode::JMP: execJMP(instr);             break;
            case Opcode::JPC: execJPC(instr);             break;
            case Opcode::OPR: execOPR(instr, out);        break;
            case Opcode::RET: execRET();                  break;
            default:
                throw RuntimeError("Opcode tidak dikenal di instruksi " + std::to_string(pc - 1));
        }
    }
}

void Interpreter::push(int val) {
    if (sp + 1 >= STACK_SIZE) throw RuntimeError("Stack Overflow: stack penuh (size=" + std::to_string(STACK_SIZE) + ")");
    stack[++sp] = val;
}

int Interpreter::pop() {
    if (sp < 0) throw RuntimeError("Stack Underflow: mencoba pop dari stack kosong");
    return stack[sp--];
}

int Interpreter::top() const {
    if (sp < 0) throw RuntimeError("Stack Underflow: stack kosong saat membaca top");
    return stack[sp];
}

int Interpreter::base(int levelDiff) const {
    int b = bp;

    for (int i = 0; i < levelDiff; i++) {
        if (b < 0 || b >= STACK_SIZE) throw RuntimeError("Invalid base pointer saat traversal level " + std::to_string(i));
        b = stack[b];
    }
    return b;
}

void Interpreter::checkStackOverflow() const {
    if (sp + 1 >= STACK_SIZE) throw RuntimeError("Stack Overflow: ukuran maksimum stack tercapai");
}

void Interpreter::checkStackUnderflow() const {
    if (sp < 0) throw RuntimeError("Stack Underflow: stack kosong");
}

void Interpreter::checkDivision(int divisor) const {
    if (divisor == 0) throw RuntimeError("Division by Zero: pembagian dengan nol");
}

void Interpreter::checkJumpTarget(int target, int codeSize) const {
    if (target < 0 || target >= codeSize)
        throw RuntimeError("Invalid Jump Target: alamat " + std::to_string(target) + " di luar range [0," + std::to_string(codeSize - 1) + "]");
}

void Interpreter::checkStackIndex(int idx) const {
    if (idx < 0 || idx >= STACK_SIZE) throw RuntimeError("Out-of-bounds Stack Access: indeks " + std::to_string(idx));
}

// ========================== executors =========================

void Interpreter::execLIT(Bytecode instr) {
    push(instr.getTarget());
}

void Interpreter::execLOD(Bytecode instr) {
    int b = base(instr.getLevel());
    int addr = b + instr.getTarget();
    checkStackIndex(addr);
    push(stack[addr]);
}

void Interpreter::execSTO(Bytecode instr) {
    int val = pop();
    int b = base(instr.getLevel());
    int addr = b + instr.getTarget();
    checkStackIndex(addr);
    stack[addr] = val;
}

void Interpreter::execCAL(Bytecode instr, const std::vector<Bytecode>& code) {
    callDepth++;
    if (callDepth > MAX_CALL_DEPTH)
        throw RuntimeError("Stack Overflow: kedalaman call melebihi batas " + std::to_string(MAX_CALL_DEPTH) + " (infinite recursion?)");

    checkJumpTarget(instr.getTarget(), code.size());

    push(base(instr.getLevel()));
    push(bp);
    push(pc);

    bp = sp - 2;
    pc = instr.getTarget();
}

void Interpreter::execINT(Bytecode instr) {
    int newSp = bp + instr.getTarget() - 1;
    if (newSp >= STACK_SIZE)
        throw RuntimeError("Stack Overflow: INT " + instr.opcodeToStr() + " melebihi ukuran stack");

    for (int i = bp; i <= newSp; i++)
        stack[i] = 0;
    sp = newSp;
}

void Interpreter::execJMP(Bytecode instr) {
    pc = instr.getTarget();
}

void Interpreter::execJPC(Bytecode instr) {
    int cond = pop();
    if (cond == 0)
        pc = instr.getTarget();
}

void Interpreter::execRET() {
    if (bp == 0 && callDepth == 0) {
        pc = -1;
        return;
    }

    int retAddr     = stack[bp + 2];
    int callerBp    = stack[bp + 1]; 

    sp = bp - 1; 
    bp = callerBp;
    pc = retAddr;

    if (callDepth > 0) callDepth--;
}

void Interpreter::execOPR(Bytecode instr, std::ostream& out) {
    OprCode opr = static_cast<OprCode>(instr.getTarget()); 

    switch (opr) {
        case OprCode::NEG: {
            int a = pop();
            push(-a);
            break;
        }
        case OprCode::ADD: {
            int b = pop();
            int a = pop();
            push(a + b);
            break;
        }
        case OprCode::SUB: {
            int b = pop();
            int a = pop();
            push(a - b);
            break;
        }
        case OprCode::MUL: {
            int b = pop();
            int a = pop();
            push(a * b);
            break;
        }
        case OprCode::DIV: {
            int b = pop();
            int a = pop();
            checkDivision(b);
            push(a / b);
            break;
        }
        case OprCode::MOD: {
            int b = pop();
            int a = pop();
            checkDivision(b);
            push(a % b);
            break;
        }
        case OprCode::EQL: {
            int b = pop();
            int a = pop();
            push(a == b ? 1 : 0);
            break;
        }
        case OprCode::NEQ: {
            int b = pop();
            int a = pop();
            push(a != b ? 1 : 0);
            break;
        }
        case OprCode::LSS: {
            int b = pop();
            int a = pop();
            push(a < b ? 1 : 0);
            break;
        }
        case OprCode::GEQ: {
            int b = pop();
            int a = pop();
            push(a >= b ? 1 : 0);
            break;
        }
        case OprCode::GTR: {
            int b = pop();
            int a = pop();
            push(a > b ? 1 : 0);
            break;
        }
        case OprCode::LEQ: {
            int b = pop();
            int a = pop();
            push(a <= b ? 1 : 0);
            break;
        }
        case OprCode::WRT: {
            int val = pop();
            out << val;
            break;
        }
        case OprCode::WRTLN: {
            int val = pop();
            out << val << "\n";
            break;
        }
        default:
            throw RuntimeError("OPR tidak dikenal: " + instr.opcodeToStr());
    }
}