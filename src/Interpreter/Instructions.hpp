#pragma once
#include <vector>
#include <string>
#include <stdexcept>

enum class Opcode {
    LIT,
    LITS,
    LITR,
    LOD,
    STO,
    CAL,
    INT,
    JMP,
    JPC,
    OPR,
    RET,
    RETVAL,
    LODI,   // Load Indirect: pops address from stack, pushes stack[bp + addr]
    STOI    // Store Indirect: pops address then value, stores value at stack[bp + addr]
};

enum class OprCode {
    NEG = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    MOD = 6,
    EQL = 7,
    NEQ = 8,
    LSS = 9,
    GEQ = 10,
    GTR = 11,
    LEQ = 12,
    WRT = 13,
    WRTLN = 14
};

class Bytecode {
private:
    int index;
    Opcode opcode;
    int level;
    int target;
public:
    Bytecode(int index, Opcode op, int lev, int oper)
        : index(index), opcode(op), level(lev), target(oper) {}
    int getIndex() { return index; }
    Opcode getOp() { return opcode; }
    int getLevel() { return level; }
    int getTarget() { return target; }
    void setTarget(int newTarget) { this->target = newTarget; }
    std::string opcodeToStr() const {
        switch (opcode) {
            case Opcode::LIT:    return "LIT";
            case Opcode::LITS:   return "LITS";
            case Opcode::LITR:   return "LITR";
            case Opcode::LOD:    return "LOD";
            case Opcode::STO:    return "STO";
            case Opcode::CAL:    return "CAL";
            case Opcode::INT:    return "INT";
            case Opcode::JMP:    return "JMP";
            case Opcode::JPC:    return "JPC";
            case Opcode::OPR:    return "OPR";
            case Opcode::RET:    return "RET";
            case Opcode::RETVAL: return "RETVAL";
            case Opcode::LODI:   return "LODI";
            case Opcode::STOI:   return "STOI";
            default: return "???";
        }
    }
    std::string toString(int lineNum) const {
        return std::to_string(lineNum) + " "
             + opcodeToStr() + " "
             + std::to_string(level) + " "
             + std::to_string(target);
    }
};

inline Opcode strToOpcode(const std::string& s) {
    if (s == "LIT")    return Opcode::LIT;
    if (s == "LITS")   return Opcode::LITS;
    if (s == "LITR")   return Opcode::LITR;
    if (s == "LOD")    return Opcode::LOD;
    if (s == "STO")    return Opcode::STO;
    if (s == "CAL")    return Opcode::CAL;
    if (s == "INT")    return Opcode::INT;
    if (s == "JMP")    return Opcode::JMP;
    if (s == "JPC")    return Opcode::JPC;
    if (s == "OPR")    return Opcode::OPR;
    if (s == "RET")    return Opcode::RET;
    if (s == "RETVAL") return Opcode::RETVAL;
    if (s == "LODI")   return Opcode::LODI;
    if (s == "STOI")   return Opcode::STOI;
    throw std::runtime_error("Unknown opcode: " + s);
}