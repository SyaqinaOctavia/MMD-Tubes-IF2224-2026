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
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROGRAM);
    std::shared_ptr<TreeNode> childA = programHeader();
    if(!childA){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childB = declarationPart();
    if(!childB){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childC = compoundStatement();
    if(!childC){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childD = terminal(Symbol::period);
    if(!childD){ currentToken = start; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    return ptr;
}

std::shared_ptr<TreeNode> Parser::programHeader(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROGRAM_HEADER);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::programsy);
    if(!childA){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::semicolon);
    if(!childC){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    
    return ptr;
}