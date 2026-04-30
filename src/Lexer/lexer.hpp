#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <fstream>
#include <string>
#include "DFA.hpp"
#include "../Token.hpp"
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
        std::vector<Token> generateToken(string srcFile);
        bool generateTokenToFile(string srcFile, string destFile);
};

#endif