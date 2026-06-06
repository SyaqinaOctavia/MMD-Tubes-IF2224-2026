#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include "Instructions.hpp"

class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& msg)
        : std::runtime_error("Runtime Error: " + msg) {}
};

static constexpr int STRING_TAG = 0x40000000;
static constexpr int REAL_TAG   = 0x20000000;

inline bool isStringVal(int v)  { return v >= 0 && (v & STRING_TAG) != 0; }
inline int  makeStringVal(int i) { return STRING_TAG | i; }
inline int  getStringIdx(int v)  { return v & ~STRING_TAG; }

inline bool isRealVal(int v)    { return v >= 0 && (v & REAL_TAG) != 0; }
inline int  makeRealVal(int i)  { return REAL_TAG | i; }
inline int  getRealIdx(int v)   { return v & ~REAL_TAG; }

class Interpreter {
public:
    static const int STACK_SIZE  = 10000;
    static const int MAX_CALL_DEPTH = 500;
    Interpreter() : stack(STACK_SIZE, 0), sp(-1), bp(0), pc(0), callDepth(0), returnValue(0), lastCallNArgs(0) {}
    void setStringTable(const std::vector<std::string>& st) { stringTable = st; }
    void setRealValues(const std::vector<double>& rv) { realValues = rv; }
    void execute(std::vector<Bytecode>& code);
    void execute(std::vector<Bytecode>& code, std::ostream& out);

private:
    std::vector<int> stack;
    std::vector<std::string> stringTable;
    std::vector<double> realValues;
    int sp;
    int bp;
    int pc;
    int callDepth;
    int returnValue;   // saved return value from the last function call
    int lastCallNArgs; // number of args of the last CAL (saved at CAL time, used by RETVAL)

    void push(int val);
    int  pop();
    int  top() const;

    int base(int levelDiff) const;

    void printVal(int val, std::ostream& out) const;

    void execLIT(Bytecode instr);
    void execLITS(Bytecode instr);
    void execLITR(Bytecode instr);
    void execLOD(Bytecode instr);
    void execSTO(Bytecode instr);
    void execCAL(Bytecode instr, const std::vector<Bytecode>& code);
    void execINT(Bytecode instr);
    void execJMP(Bytecode instr);
    void execJPC(Bytecode instr);
    void execOPR(Bytecode instr, std::ostream& out);
    void execRET();
    void execRETVAL();
    void execLODI(Bytecode instr);
    void execSTOI(Bytecode instr);

    // Helper for real arithmetic
    double decodeValue(int val) const;
    int encodeRealResult(double val);

    void checkStackOverflow()  const;
    void checkStackUnderflow() const;
    void checkDivision(int divisor) const;
    void checkJumpTarget(int target, int codeSize) const;
    void checkStackIndex(int idx) const;
};