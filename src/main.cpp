#include "lexer.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3){
        cout << "[USAGE] : ./main <input file> <output file>" << endl;
        return 0;
    }
    Lexer lex;
    lex.generateToken(argv[1],argv[2]);
}