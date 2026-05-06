#include "Parser.hpp"
#include <map>
#include <fstream>
#include <exception>

std::shared_ptr<TreeNode> Parser::terminal(Symbol symbol){
    std::shared_ptr<TreeNode> ptr = nullptr;
    if(tokens[currentToken].getTokenType() == symbol){
        ptr = std::make_shared<TreeNode>(symbol);
        currentToken++;
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
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::RANGE);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::CONSTANT);
    if(!childA){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::period);
    if(!childB){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::period);
    if(!childC){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childD = terminal(Symbol::CONSTANT);
    if(!childD){ currentToken = start; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::enumerated(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::ENUMERATED);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::lparent);
    if(!childA){ currentToken = start; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; return nullptr;}
    
    // Parse remaining comma and identifier
    std::vector<std::shared_ptr<TreeNode>> parseContainer;
    while( currentToken+1 < tokens.size() && tokens[currentToken].getTokenType() == Symbol::comma ){
        std::shared_ptr<TreeNode> parseComma = terminal(Symbol::comma);
        if( !parseComma ) { currentToken = start; return nullptr; }
        std::shared_ptr<TreeNode> parseIdent = terminal(Symbol::ident);
        if( !parseIdent ) { currentToken = start; return nullptr; }

        // Add to container
        parseContainer.push_back(parseComma);
        parseContainer.push_back(parseIdent);
    }
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::rparent);
    if(!childC){ currentToken = start; return nullptr;}
    
    ptr->addChild(childA);
    for(auto parsed : parseContainer) ptr->addChild(parsed);
    ptr->addChild(childB);
    ptr->addChild(childC);
    
    return ptr;
}

// <record-type> -> recordsy + field-list + endsy
std::shared_ptr<TreeNode> Parser::recordType(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::RECORD_TYPE);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::recordsy);
    if(!childA){ currentToken = start; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = fieldList();
    if(!childB){ currentToken = start; return nullptr;}
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::endsy);
    if(!childC){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    
    return ptr;
}

// <field-list> -> field-part + (semicolon + field-part)*
std::shared_ptr<TreeNode> Parser::fieldList(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FIELD_LIST);

    std::shared_ptr<TreeNode> childA = fieldPart();
    if(!childA){ currentToken = start; return nullptr;}
    
    // Parse remaining semicolon and field-part
    std::vector<std::shared_ptr<TreeNode>> parseContainer;
    while( currentToken+1 < tokens.size() && tokens[currentToken].getTokenType() == Symbol::semicolon ){
        std::shared_ptr<TreeNode> parseSemicolon = terminal(Symbol::semicolon);
        if( !parseSemicolon ) { currentToken = start; return nullptr; }
        std::shared_ptr<TreeNode> parseField = fieldPart();
        if( !parseField ) { currentToken = start; return nullptr; }

        // Add to container
        parseContainer.push_back(parseSemicolon);
        parseContainer.push_back(parseField);
    }
    
    ptr->addChild(childA);
    for(auto parsed : parseContainer) ptr->addChild(parsed);
    
    return ptr;
}

// <field-part> -> identifier-list + colon + type
std::shared_ptr<TreeNode> Parser::fieldPart(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FIELD_PART);

    std::shared_ptr<TreeNode> childA = identifierList();
    if(!childA){ currentToken = start; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = terminal(Symbol::colon);
    if(!childB){ currentToken = start; return nullptr;}
    
    std::shared_ptr<TreeNode> childC = typeNode();
    if(!childC){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);

    return ptr;
}

// <subprogram-declaration> -> procedure-declaration | function-declaration
std::shared_ptr<TreeNode> Parser::subprogramDeclaration(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::SUBPROGRAM_DECLARATION);

    std::shared_ptr<TreeNode> childA = procedureDeclaration();
    if(childA){
        ptr->addChild(childA);
        return ptr;
    }
    
    std::shared_ptr<TreeNode> childB = functionDeclaration();
    if(childB){
        ptr->addChild(childB);
        return ptr;
    }

    currentToken = start;
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
