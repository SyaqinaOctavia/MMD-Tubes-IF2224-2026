#include "Parser.hpp"
#include <map>
#include <fstream>
#include <exception>

std::shared_ptr<TreeNode> Parser::terminal(Symbol symbol){
    std::shared_ptr<TreeNode> ptr = nullptr;
    if(tokens[currentToken].getTokenType() == symbol){
        ptr = std::make_shared<TreeNode>(symbol);
    }
    return ptr;
}

std::shared_ptr<TreeNode> Parser::programNode(){
    std::shared_ptr<TreeNode> ptr = nullptr;
    
    return ptr;
}