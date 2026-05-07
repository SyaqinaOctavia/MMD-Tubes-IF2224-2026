#include "Lexer/lexer.hpp"
#include "Parser.hpp"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3){
        cout << "[USAGE] : ./main <option> <input file> <output file>" << endl;
        cout << "option :" << endl;
        cout << "         1. Lexer (input with code, output as tokens)" << endl;
        cout << "         2. Lexer (input with tokens, output as parse tree)" << endl;
        cout << "         3. Lexer and Parser (input with code, output as parse tree)" << endl;
        return 0;
    }
    if(std::strcmp(argv[1], "1") == 0){
        Lexer lex;
        lex.generateTokenToFile(argv[2],argv[3]);
    } 
    else if(std::strcmp(argv[1], "2") == 0){
        std::vector<Token> tokens = Token::readTokens(argv[2]);
        Parser parser(tokens);
        std::shared_ptr<TreeNode> tree = parser.buildTree();
        if(tree != nullptr)
            tree->outputTree(argv[3]);
    }
    else if(std::strcmp(argv[1], "3") == 0){
        Lexer lex;
        std::vector<Token> tokens = lex.generateToken(argv[2]);
        Parser parser(tokens);
        std::shared_ptr<TreeNode> tree = parser.buildTree();
        if(tree != nullptr)
            tree->outputTree(argv[3]);
        // not implemented yet
    }
    else {
        cout << "OPTION INVALID!" << endl;
    }
}