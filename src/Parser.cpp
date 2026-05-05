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

std::shared_ptr<TreeNode> Parser::declarationPart(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::constDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::constantNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::typeDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::varDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::identifierList(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::typeNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::arrayType(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::rangeNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::enumerated(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::recordType(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::fieldList(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::fieldPart(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::subprogramDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::procedureDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::functionDeclaration(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::blockNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::formalParameterList(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::parameterGroup(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::compoundStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::statementList(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::statementNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::assignmentStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::ifStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::caseStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::caseBlock(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::whileStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::repeatStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::forStatement(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::procedureFunctionCall(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::parameterList(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::expressionNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::simpleExpression(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::termNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::factorNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::relationalOperator(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::additiveOperator(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::multiplicativeOperator(){
    return nullptr;
}
