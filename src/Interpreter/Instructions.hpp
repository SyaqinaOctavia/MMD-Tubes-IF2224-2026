#pragma once
#include <vector>

enum class Operator {
    LIT,
    LOD,
    STO,
    CAL,
    INT,
    JMP,
    JPC,
    OPR,
    RET
};

enum class OPRInst {
    NEG,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQL,
    NEQ,
    LSS,
    GEQ,
    GTR,
    LEQ,
    WRT,
    WRTLN
};

class Bytecode {
private:
    int index;
    Operator op;
    int level;
    int target;
public:
    Bytecode(int index, Operator op, int level, int target) :
        index(index), op(op), level(level), target(target) {}
    int getIndex(){ return index; }
    Operator getOp() { return op; }
    int getLevel(){ return level; }
    int getTarget(){ return target; }
    void setTarget(int newTarget){ this->target = newTarget; }
};

class InterCode {
private:
    std::vector<Bytecode> bytecodes;
public:
    void addLine(Operator op, int level, int target){
        int index = bytecodes.size();
        Bytecode newLine(index, op, level, target);
        bytecodes.push_back(newLine);
    }

    void patch(int index, int newTarget){
        bytecodes[index].setTarget(newTarget);
    }

    int size(){ return bytecodes.size(); }
    Bytecode& get(int index){ return bytecodes[index]; }
};