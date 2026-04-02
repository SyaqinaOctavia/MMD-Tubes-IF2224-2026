#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class lexer {
    private:
        char currentChar;
        bool EOP;
        ifstream scanner;
    public:
        char getCurrent(){ return currentChar; };
        bool isEndFile(){ return EOP; };
        void advance();
        void readFile(string filepath);
        void skipWhitespace();
};

#endif