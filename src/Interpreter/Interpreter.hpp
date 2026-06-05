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

class Interpreter {
public:
    static const int STACK_SIZE  = 10000;
    static const int MAX_CALL_DEPTH = 500;
    Interpreter() : stack(STACK_SIZE, 0), sp(-1), bp(0), pc(0), callDepth(0) {}
    void execute(std::vector<Bytecode>& code);
    void execute(std::vector<Bytecode>& code, std::ostream& out);

private:
    std::vector<int> stack;
    int sp;
    int bp;
    int pc;
    int callDepth;

    void push(int val);
    int  pop();
    int  top() const;

    int base(int levelDiff) const;

    void execLIT(Bytecode instr);
    void execLOD(Bytecode instr);
    void execSTO(Bytecode instr);
    void execCAL(Bytecode instr, const std::vector<Bytecode>& code);
    void execINT(Bytecode instr);
    void execJMP(Bytecode instr);
    void execJPC(Bytecode instr);
    void execOPR(Bytecode instr, std::ostream& out);
    void execRET();

    void checkStackOverflow()  const;
    void checkStackUnderflow() const;
    void checkDivision(int divisor) const;
    void checkJumpTarget(int target, int codeSize) const;
    void checkStackIndex(int idx) const;
};