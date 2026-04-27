#include "lexer.hpp"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3){
        cout << "[USAGE] : ./main <option> <input file> <output file>" << endl;
        cout << "option :" << endl;
        cout << "         1. Lexer (input with code, output as tokens)" << endl;
        cout << "         2. Parser (input with tokens, output as parse tree)" << endl;
        return 0;
    }
    if(std::strcmp(argv[1], "1") == 0){
        Lexer lex;
        lex.generateToken(argv[2],argv[3]);
    }
    else if(std::strcmp(argv[1], "2") == 0){
        // not implemented yet
    }
    else {
        cout << "OPTION INVALID!" << endl;
    }
}