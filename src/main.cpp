#include "Lexer/lexer.hpp"
#include "Parser/Parser.hpp"
#include "Parser/ParseTree.hpp"
#include "Semantic/ASTer.hpp"
#include "Semantic/SymbolTable.hpp"
#include "Semantic/SemanticAnalyzer.hpp"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3){
        cout << "[USAGE] : ./main <option> <input file> <output file>" << endl;
        cout << "option :" << endl;
        cout << "         1. Lexer (input with code, output as tokens)" << endl;
        cout << "         2a. Parser (input with tokens, output as parse tree)" << endl;
        cout << "         2b. Lexer and Parser (input with code, output as parse tree)" << endl;
        cout << "         3a. Semantic Analyzer (input with parse tree, output as AST)" << endl;
        cout << "         3b. Lexer, Parser, and Semantic Analyzer (input with code, output as AST)" << endl;
        return 0;
    }
    if(std::strcmp(argv[1], "1") == 0){
        Lexer lex;
        lex.generateTokenToFile(argv[2],argv[3]);
    } 
    else if(std::strcmp(argv[1], "2a") == 0){
        std::vector<Token> tokens = Token::readTokens(argv[2]);
        Parser parser(tokens);
        std::shared_ptr<TreeNode> tree = parser.buildTree();
        if(tree != nullptr)
            tree->outputTree(argv[3]);
    }
    else if(std::strcmp(argv[1], "2b") == 0){
        Lexer lex;
        std::vector<Token> tokens = lex.generateToken(argv[2]);
        Parser parser(tokens);
        std::shared_ptr<TreeNode> tree = parser.buildTree();
        if(tree != nullptr)
            tree->outputTree(argv[3]);
    }
    else if(std::strcmp(argv[1], "3a") == 0){
        std::shared_ptr<TreeNode> tree = TreeNode::readTreeFromFile(argv[2]);
        if(tree != nullptr){
            ASTer aster;
            std::shared_ptr<ASTNode> astree = aster.buildProgramNode(tree);
            if(astree == nullptr) cout << "error while building AST tree"; // unreachable sih but who knows
            SymbolTable symtab;
            SemanticAnalyzer seman(symtab);
            seman.analyzeAndOutput(astree, argv[3]);
        }
    }
    else if(std::strcmp(argv[1], "3b") == 0){
        Lexer lex;
        std::vector<Token> tokens = lex.generateToken(argv[2]);
        Parser parser(tokens);
        std::shared_ptr<TreeNode> tree = parser.buildTree();
        if(tree != nullptr){
            ASTer aster;
            std::shared_ptr<ASTNode> astree = aster.buildProgramNode(tree);
            if(astree == nullptr) cout << "error while building AST tree"; // unreachable sih but who knows
            SymbolTable symtab;
            SemanticAnalyzer seman(symtab);
            seman.analyzeAndOutput(astree, argv[3]);
        }
    }
    else {
        cout << "OPTION INVALID!" << endl;
    }
}