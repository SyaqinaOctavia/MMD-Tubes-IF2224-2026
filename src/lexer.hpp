#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <fstream>
#include <string>
#include "DFA.hpp"
using namespace std;

class Lexer {
    private:
        char currentChar;
        int line;
        bool EOP;
        ifstream scanner;
    public:
        Lexer();
        char getCurrent(){ return currentChar; };
        bool isEndFile(){ return EOP; };
        void advance();
        void readFile(string filepath);
        void skipWhitespace();
        bool generateToken(string srcFile, string destFile);
};

#endif