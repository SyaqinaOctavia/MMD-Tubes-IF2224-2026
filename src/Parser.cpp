#include "Parser.hpp"
#include <map>
#include <fstream>
#include <iostream>
#include <exception>

std::shared_ptr<TreeNode> Parser::buildTree(){
    std::shared_ptr<TreeNode> tree = programNode();
    if(tree == nullptr){
        std::cout << "Parsing failed," << std::endl;
        std::cout << "          expected: " << toString(expected) << std::endl;
        std::cout << "          received: " << toString(received->getTokenType()) << std::endl;
        std::cout << "          at production " << toString(currentProd) << std::endl;
        std::cout << "          at line " << received->getLine() << std::endl;
    }
    return tree;
}

std::shared_ptr<TreeNode> Parser::terminal(Symbol symbol){
    std::shared_ptr<TreeNode> ptr = nullptr;
    if(tokens[currentToken].getTokenType() == symbol){
        ptr = std::make_shared<TreeNode>(symbol);
        ptr->setValue(tokens[currentToken].getValue());
        currentToken++;
    } else {
        // std::cout << "Temporary fault at " << toString(currentProd) << " : (expected " << toString(symbol) << ", found " << toString(tokens[currentToken].getTokenType()) << ", line " << received->getLine() <<  ")" << std::endl;
        received = &tokens[currentToken];
        expected = symbol;
    }
    return ptr;
}

// <program> -> program-header + declaration-part + compound-statement + period
std::shared_ptr<TreeNode> Parser::programNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PROGRAM;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROGRAM);
    std::shared_ptr<TreeNode> childA = programHeader();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childB = declarationPart();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = compoundStatement();
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childD = terminal(Symbol::period);
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    currentProd = prevProd; return ptr;
}

// <declaration-part> -> (const-declaration)* + (type-declaration)* + 
//                       (var-declaration)* + (subprogram-declaration)*
std::shared_ptr<TreeNode> Parser::programHeader(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PROGRAM_HEADER;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROGRAM_HEADER);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::programsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::semicolon);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    
    currentProd = prevProd; return ptr;
}

// <const-declaration> -> constsy + (ident + eql + constant + semicolon)+
std::shared_ptr<TreeNode> Parser::declarationPart(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::DECLARATION_PART;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::DECLARATION_PART);
    // (const-declaration)*
    std::shared_ptr<TreeNode> child = constDeclaration();
    while(child != nullptr){
        ptr->addChild(child);
        child = constDeclaration();
    }
    // (type-declaration)*
    child = typeDeclaration();
    while(child != nullptr){
        ptr->addChild(child);
        child = typeDeclaration();
    }
    // (var-declaration)
    child = varDeclaration();
    while(child != nullptr){
        ptr->addChild(child);
        child = varDeclaration();
    }
    // (subprogram-declaration)
    child = subprogramDeclaration();
    while(child != nullptr){
        ptr->addChild(child);
        child = subprogramDeclaration();
    }
    
    currentProd = prevProd; return ptr;
}

// <const-declaration> -> typesy + (ident + eql + type + semicolon)+
std::shared_ptr<TreeNode> Parser::constDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::CONST_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::CONST_DECLARATION);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::constsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::eql);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childD = constantNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);

    while(currentToken < tokens.size() && tokens[currentToken].getTokenType() == Symbol::ident){
        start = currentToken;

        childB = terminal(Symbol::ident);
        if(!childB){ currentToken = start; break; }
        childC = terminal(Symbol::eql);
        if(!childC){ currentToken = start; break; }
        childD = constantNode();
        if(!childD){ currentToken = start; break; }
        childE = terminal(Symbol::semicolon);
        if(!childE){ currentToken = start; break; }

        ptr->addChild(childB);
        ptr->addChild(childC);
        ptr->addChild(childD);
        ptr->addChild(childE);
    }
    currentProd = prevProd; return ptr;
}

// <constant> -> charcon | string | [(plus | minus)? + (ident | intcon | realcon)]
std::shared_ptr<TreeNode> Parser::constantNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::CONSTANT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::CONSTANT);

    std::shared_ptr<TreeNode> child = terminal(Symbol::charcon);
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    child = terminal(Symbol::string);
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    std::shared_ptr<TreeNode> childSign = terminal(Symbol::plus);
    if(childSign == nullptr){
        childSign = terminal(Symbol::minus);
    }

    child = terminal(Symbol::ident);
    if(child != nullptr){
        if(childSign != nullptr) ptr->addChild(childSign);
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }
    child = terminal(Symbol::intcon);
    if(child != nullptr){
        if(childSign != nullptr) ptr->addChild(childSign);
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }
    child = terminal(Symbol::realcon);
    if(child != nullptr){
        if(childSign != nullptr) ptr->addChild(childSign);
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }
    currentToken = start;
    currentProd = prevProd; return nullptr;
}

// <type-declaration> -> typesy + (ident + eql + type + semicolon)+
std::shared_ptr<TreeNode> Parser::typeDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::TYPE_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::TYPE_DECLARATION);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::typesy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::eql);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childD = typeNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);

    while(currentToken < tokens.size() && tokens[currentToken].getTokenType() == Symbol::ident){
        start = currentToken;

        childB = terminal(Symbol::ident);
        if(!childB){ currentToken = start; break; }
        childC = terminal(Symbol::eql);
        if(!childC){ currentToken = start; break; }
        childD = typeNode();
        if(!childD){ currentToken = start; break; }
        childE = terminal(Symbol::semicolon);
        if(!childE){ currentToken = start; break; }

        ptr->addChild(childB);
        ptr->addChild(childC);
        ptr->addChild(childD);
        ptr->addChild(childE);
    }
    currentProd = prevProd; return ptr;
}

// <var-declaration> ->  varsy + (identifier-list + colon + type + semicolon)+
std::shared_ptr<TreeNode> Parser::varDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::VAR_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::VAR_DECLARATION);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::varsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    std::shared_ptr<TreeNode> childB = identifierList();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::colon);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childD = typeNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);

    while(currentToken < tokens.size() && tokens[currentToken].getTokenType() == Symbol::ident){
        start = currentToken;

        childB = identifierList();
        if(!childB){ currentToken = start; break; }
        childC = terminal(Symbol::colon);
        if(!childC){ currentToken = start; break; }
        childD = typeNode();
        if(!childD){ currentToken = start; break; }
        childE = terminal(Symbol::semicolon);
        if(!childE){ currentToken = start; break; }

        ptr->addChild(childB);
        ptr->addChild(childC);
        ptr->addChild(childD);
        ptr->addChild(childE);
    }
    currentProd = prevProd; return ptr;
}

// <identifier-list> -> ident (comma + ident)*
std::shared_ptr<TreeNode> Parser::identifierList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::IDENTIFIER_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::IDENTIFIER_LIST);
    std::shared_ptr<TreeNode> childA = terminal(Symbol::ident);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    while(currentToken < tokens.size() && tokens[currentToken].getTokenType() == Symbol::comma){
        start = currentToken;
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        std::shared_ptr<TreeNode> childC = terminal(Symbol::ident);
        if(childC == nullptr){ currentToken = start; break; }

        ptr->addChild(childB);
        ptr->addChild(childC);
    }

    currentProd = prevProd; return ptr;
}

// <type> -> ident | array-type | range | enumerated | record-type
std::shared_ptr<TreeNode> Parser::typeNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::TYPE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::TYPE);

    std::shared_ptr<TreeNode> child = terminal(Symbol::ident);
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    child = arrayType();
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    child = rangeNode();
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    child = enumerated();
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    child = recordType();
    if(child != nullptr){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }

    currentToken = start;
    currentProd = prevProd; return nullptr;
}

// <<array-type> -> arraysy + lbrack + (range | ident) + rbrack + ofsy + type
std::shared_ptr<TreeNode> Parser::arrayType(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::ARRAY_TYPE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::ARRAY_TYPE);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::arraysy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = terminal(Symbol::lbrack);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}

    std::shared_ptr<TreeNode> childC = rangeNode();
    if(!childC){ 
        childC = terminal(Symbol::ident);
        if(!childC){
            currentToken = start; 
            currentProd = prevProd; return nullptr;
        }
    }

    std::shared_ptr<TreeNode> childD = terminal(Symbol::rbrack);
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}

    std::shared_ptr<TreeNode> childE = terminal(Symbol::ofsy);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}

    std::shared_ptr<TreeNode> childF = typeNode();
    if(!childF){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);
    ptr->addChild(childF);
    currentProd = prevProd; return ptr;
}

// <range> -> constant + period + period + constant
std::shared_ptr<TreeNode> Parser::rangeNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::RANGE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::RANGE);

    std::shared_ptr<TreeNode> childA = constantNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::period);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childC = terminal(Symbol::period);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childD = constantNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    
    currentProd = prevProd; return ptr;
}

// <enumerated> ->  lparent + ident + (comma + ident)* + rparent
std::shared_ptr<TreeNode> Parser::enumerated(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::ENUMERATED;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::ENUMERATED);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::lparent);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    // Parse remaining comma and identifier
    std::vector<std::shared_ptr<TreeNode>> parseContainer;
    while( currentToken+1 < tokens.size() && tokens[currentToken].getTokenType() == Symbol::comma ){
        std::shared_ptr<TreeNode> parseComma = terminal(Symbol::comma);
        if( !parseComma ) { currentToken = start; currentProd = prevProd; return nullptr; }
        std::shared_ptr<TreeNode> parseIdent = terminal(Symbol::ident);
        if( !parseIdent ) { currentToken = start; currentProd = prevProd; return nullptr; }

        // Add to container
        parseContainer.push_back(parseComma);
        parseContainer.push_back(parseIdent);
    }
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::rparent);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    for(auto parsed : parseContainer) ptr->addChild(parsed);
    ptr->addChild(childC);
    
    currentProd = prevProd; return ptr;
}

// <record-type> -> recordsy + field-list + endsy
std::shared_ptr<TreeNode> Parser::recordType(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::RECORD_TYPE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::RECORD_TYPE);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::recordsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = fieldList();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::endsy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    
    currentProd = prevProd; return ptr;
}

// <field-list> -> field-part + (semicolon + field-part)*
std::shared_ptr<TreeNode> Parser::fieldList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FIELD_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FIELD_LIST);

    std::shared_ptr<TreeNode> childA = fieldPart();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    // Parse remaining semicolon and field-part
    std::vector<std::shared_ptr<TreeNode>> parseContainer;
    while( currentToken+1 < tokens.size() && tokens[currentToken].getTokenType() == Symbol::semicolon ){
        std::shared_ptr<TreeNode> parseSemicolon = terminal(Symbol::semicolon);
        if( !parseSemicolon ) { currentToken = start; currentProd = prevProd; return nullptr; }
        std::shared_ptr<TreeNode> parseField = fieldPart();
        if( !parseField ) { currentToken = start; currentProd = prevProd; return nullptr; }

        // Add to container
        parseContainer.push_back(parseSemicolon);
        parseContainer.push_back(parseField);
    }
    
    ptr->addChild(childA);
    for(auto parsed : parseContainer) ptr->addChild(parsed);
    
    currentProd = prevProd; return ptr;
}

// <field-part> -> identifier-list + colon + type
std::shared_ptr<TreeNode> Parser::fieldPart(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FIELD_PART;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FIELD_PART);

    std::shared_ptr<TreeNode> childA = identifierList();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = terminal(Symbol::colon);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childC = typeNode();
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);

    currentProd = prevProd; return ptr;
}

// <subprogram-declaration> -> procedure-declaration | function-declaration
std::shared_ptr<TreeNode> Parser::subprogramDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::SUBPROGRAM_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::SUBPROGRAM_DECLARATION);

    std::shared_ptr<TreeNode> childA = procedureDeclaration();
    if(childA){
        ptr->addChild(childA);
        currentProd = prevProd; return ptr;
    }
    
    std::shared_ptr<TreeNode> childB = functionDeclaration();
    if(childB){
        ptr->addChild(childB);
        currentProd = prevProd; return ptr;
    }

    currentToken = start;
    currentProd = prevProd; return nullptr;
}

// <procedure-declaration> -> proceduresy + ident + (formal-parameter-list)? + semicolon + block + semicolon
std::shared_ptr<TreeNode> Parser::procedureDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PROCEDURE_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROCEDURE_DECLARATION);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::proceduresy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    // Optional parameter list
    std::shared_ptr<TreeNode> parseParams = formalParameterList();
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::semicolon);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childD = blockNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    if(parseParams) ptr->addChild(parseParams);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);
    
    currentProd = prevProd; return ptr;
}

// <function-declaration> -> functionsy + ident + (formal-parameter-list)? + colon + ident + semicolon + block + semicolon
std::shared_ptr<TreeNode> Parser::functionDeclaration(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FUNCTION_DECLARATION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FUNCTION_DECLARATION);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::functionsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    // Optional parameter list
    std::shared_ptr<TreeNode> parseParams = formalParameterList();
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::colon);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childD = terminal(Symbol::ident);
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childF = blockNode();
    if(!childF){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childG = terminal(Symbol::semicolon);
    if(!childG){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    if(parseParams) ptr->addChild(parseParams);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);
    ptr->addChild(childF);
    ptr->addChild(childG);
    
    currentProd = prevProd; return ptr;
}

// block -> declaration-part + compound-statement
std::shared_ptr<TreeNode> Parser::blockNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::BLOCK;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::BLOCK);

    std::shared_ptr<TreeNode> childA = declarationPart();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = compoundStatement(); 
    ptr->addChild(childA);
    ptr->addChild(childB);

    currentProd = prevProd; return ptr;
}

// <formal-parameter-list> -> lparent + parameter-group + (semicolon + parameter-group)* + rparent
std::shared_ptr<TreeNode> Parser::formalParameterList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FORMAL_PARAMETER_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FORMAL_PARAMETER_LIST);

    std::shared_ptr<TreeNode> childA = terminal(Symbol::lparent);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    std::shared_ptr<TreeNode> childB = parameterGroup(); 
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    // Parse remaining semicolon and parameter-group
    std::vector<std::shared_ptr<TreeNode>> parseContainer;
    while( currentToken+1 < tokens.size() && tokens[currentToken].getTokenType() == Symbol::semicolon ){
        std::shared_ptr<TreeNode> parseSemicolon = terminal(Symbol::semicolon);
        if( !parseSemicolon ) { currentToken = start; currentProd = prevProd; return nullptr; }
        std::shared_ptr<TreeNode> parseGroup = parameterGroup();
        if( !parseGroup ) { currentToken = start; currentProd = prevProd; return nullptr; }

        // Add to container
        parseContainer.push_back(parseSemicolon);
        parseContainer.push_back(parseGroup);
    }
    
    std::shared_ptr<TreeNode> childC = terminal(Symbol::rparent);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    
    ptr->addChild(childA);
    ptr->addChild(childB);
    for(auto parsed : parseContainer) ptr->addChild(parsed);
    ptr->addChild(childC);
    
    currentProd = prevProd; return ptr;
}

// <parameter-group> -> identifier-list + colon + (ident | array-type)
std::shared_ptr<TreeNode> Parser::parameterGroup(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PARAMETER_GROUP;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PARAMETER_GROUP);

    //identifier-list
    std::shared_ptr<TreeNode> childA = identifierList();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //colon
    std::shared_ptr<TreeNode> childB = terminal(Symbol::colon);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);

    // (ident | array-type)
    if(tokens[currentToken].getTokenType() == Symbol::ident){
        std::shared_ptr<TreeNode> childC = terminal(Symbol::ident);
        if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childC);    
    }else{
        std::shared_ptr<TreeNode> childC = arrayType();
        if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childC);
    }

    currentProd = prevProd; return ptr;
}

//<compound-statement> -> beginsy + statement-list + endsy
std::shared_ptr<TreeNode> Parser::compoundStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::COMPOUND_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::COMPOUND_STATEMENT);

    //beginsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::beginsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //statement-list
    std::shared_ptr<TreeNode> childB = statementList();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);

    //endsy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::endsy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childC);

    currentProd = prevProd; return ptr;
}

// <statement-list> -> statement (semicolon + statement)*
std::shared_ptr<TreeNode> Parser::statementList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::STATEMENT_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::STATEMENT_LIST);

    //statement
    std::shared_ptr<TreeNode> childA = statementNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    
    //(semicolon + statement)*
    while (true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::semicolon);
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = statementNode();
            if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
            ptr->addChild(childC);
        } else break;  
    }

    currentProd = prevProd; return ptr;
}

// <statement> -> (assignment-statement | if-statement | case-statement | while-statement | repeat-statement | for-statement | procedure/function-call )?
std::shared_ptr<TreeNode> Parser::statementNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::STATEMENT);

    // (assignment-statement | if-statement | case-statement | while-statement | 
    // repeat-statement | for-statement | procedure/function-call )?

    std::shared_ptr<TreeNode> child = assignmentStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = ifStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = caseStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = whileStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = repeatStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = forStatement();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    child = procedureFunctionCall();
    if(child){ ptr->addChild(child); currentProd = prevProd; return ptr; }

    currentProd = prevProd; return ptr;
}

// <variable> -> ident + (component-variable)*
std::shared_ptr<TreeNode> Parser::variableNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::VARIABLE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::VARIABLE);

    //ident
    std::shared_ptr<TreeNode> childA = terminal(Symbol::ident);
    if(!childA){currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //(component-variable)*
    while (true){
        std::shared_ptr<TreeNode> childB = componentVariable();
        if(childB) ptr->addChild(childB);
        else break;
    }

    currentProd = prevProd; return ptr;
}

// <component-variable> -> (lbrack + index-list + rbrack) | (period + ident)
std::shared_ptr<TreeNode> Parser::componentVariable(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::COMPONENT_VARIABLE;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::COMPONENT_VARIABLE);

    //lbrack + index-list + rbrack
    if(tokens[currentToken].getTokenType() == Symbol::lbrack){
        std::shared_ptr<TreeNode> childA = terminal(Symbol::lbrack);
        if(!childA){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(childA);
        
        std::shared_ptr<TreeNode> childB = indexList();
        if(!childB){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(childB);
        
        std::shared_ptr<TreeNode> childC = terminal(Symbol::rbrack);
        if(!childC){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(childC);
    }
    //period + ident
    else{ 
        std::shared_ptr<TreeNode> childA = terminal(Symbol::period);
        if(!childA){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(childA);

        std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
        if(!childB){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(childB);
    }

    currentProd = prevProd; return ptr;
}

// <index-list> -> ( intcon | charcon | ident ) + ( comma + index-list )*
std::shared_ptr<TreeNode> Parser::indexList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::INDEX_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::INDEX_LIST);

    //(intcon | charcon | ident)
    std::shared_ptr<TreeNode> childA = terminal(Symbol::intcon);
    if(!childA) childA = terminal(Symbol::charcon);
    if(!childA) childA = terminal(Symbol::ident);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr; }
    ptr->addChild(childA);

    //(comma + index-list)*
    while (true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        if(childB){
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = indexList();
            if(!childC){ currentToken = start; currentProd = prevProd; return nullptr; }
            ptr->addChild(childC);
        } else break;
    }

    currentProd = prevProd; return ptr;
}

// <assignment-statement> -> variable + becomes + expression
std::shared_ptr<TreeNode> Parser::assignmentStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::ASSIGNMENT_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::ASSIGNMENT_STATEMENT);

    //variable
    std::shared_ptr<TreeNode> childA = variableNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //becomes
    std::shared_ptr<TreeNode> childB = terminal(Symbol::becomes);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);

    //expression
    std::shared_ptr<TreeNode> childC = expressionNode();
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childC);

    currentProd = prevProd; return ptr;
}

// <if-statement> -> ifsy + expression + thensy + statement + (elsy + statement)?
std::shared_ptr<TreeNode> Parser::ifStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::IF_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::IF_STATEMENT);

    //ifsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::ifsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //expression
    std::shared_ptr<TreeNode> childB = expressionNode();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);
    
    //thensy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::thensy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childC);

    //statement
    std::shared_ptr<TreeNode> childD = statementNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childD);
    
    //  (elsesy + statement)?
    if(tokens[currentToken].getTokenType() == Symbol::elsesy){
        std::shared_ptr<TreeNode> childF = terminal(Symbol::elsesy);
        if(!childF){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childF);

        //statement
        std::shared_ptr<TreeNode> childG = statementNode();
        if(!childG){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childG);
    }

    currentProd = prevProd; return ptr;
}

// <case-statement> -> casesy + expression + ofsy + case-block + endsy
std::shared_ptr<TreeNode> Parser::caseStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::CASE_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::CASE_STATEMENT);

    //casesy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::casesy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);

    //expression
    std::shared_ptr<TreeNode> childB = expressionNode();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);
    
    //ofsy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::ofsy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childC);

    //case-block
    std::shared_ptr<TreeNode> childD = caseBlock();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childD);

    //endsy
    std::shared_ptr<TreeNode> childE = terminal(Symbol::endsy);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childE);

    currentProd = prevProd; return ptr;
}

// <case-block> -> constant + (comma + constant)* + colon + statement + (semicolon + case-block?)*
std::shared_ptr<TreeNode> Parser::caseBlock(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::CASE_BLOCK;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::CASE_BLOCK);
    //constant
    std::shared_ptr<TreeNode> childA = constantNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    //(comma + constant)*
    while (true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = constantNode();
            if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
            ptr->addChild(childC);
        } else break;  
    }
    //colon
    std::shared_ptr<TreeNode> childD = terminal(Symbol::colon);
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childD);
    //statement
    std::shared_ptr<TreeNode> childE = statementNode();
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childE);
    //(semicolon + case-block?)*
    while (true){
        std::shared_ptr<TreeNode> childF = terminal(Symbol::semicolon);
        if(childF){ 
            ptr->addChild(childF);

            auto isFirstCaseBlock = [&](int pos) -> bool {
                if (pos >= (int)tokens.size()) { return false; }
                Symbol s = tokens[pos].getTokenType();
                if (s == Symbol::charcon || s == Symbol::string ||
                    s == Symbol::ident   || s == Symbol::intcon ||
                    s == Symbol::realcon) { return true; }
                if ((s == Symbol::plus || s == Symbol::minus) &&
                    pos + 1 < (int)tokens.size()) {
                    Symbol next_s = tokens[pos + 1].getTokenType();
                    return next_s == Symbol::ident  ||
                        next_s == Symbol::intcon ||
                        next_s == Symbol::realcon;
                }
                return false;
            };

            if(isFirstCaseBlock(currentToken)){
                std::shared_ptr<TreeNode> childG = caseBlock();
                if(!childG){ currentToken = start; currentProd = prevProd; return nullptr; }
                ptr->addChild(childG);
            } else break;
            
        } else break;
        
    } 
    
    currentProd = prevProd; return ptr;
}

// <while-statement> -> whilesy + expression + dosy + compound-statement + semicolon
std::shared_ptr<TreeNode> Parser::whileStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::WHILE_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::WHILE_STATEMENT);
    //whilesy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::whilesy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childB = expressionNode();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    //dosy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::dosy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    //statement
    std::shared_ptr<TreeNode> childD = compoundStatement();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    //semicolon
    std::shared_ptr<TreeNode> childE = terminal(Symbol::semicolon);
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);

    currentProd = prevProd; return ptr;
}

// <repeat-statement> -> repeatsy + statement-list + untilsy + expression
std::shared_ptr<TreeNode> Parser::repeatStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::REPEAT_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::REPEAT_STATEMENT);
    //repeatsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::repeatsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    //statement-list
    std::shared_ptr<TreeNode> childB = statementList();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    //untilsy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::untilsy);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childD = expressionNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    
    currentProd = prevProd; return ptr;
}

// <for-statement> -> forsy + ident + becomes + expression + ( tosy | downtosy) + expression + dosy + compound-statement
std::shared_ptr<TreeNode> Parser::forStatement(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FOR_STATEMENT;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FOR_STATEMENT);
    //forsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::forsy);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    //ident
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    //becomes
    std::shared_ptr<TreeNode> childC = terminal(Symbol::becomes);
    if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childD = expressionNode();
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    //( tosy | downtosy)
    std::shared_ptr<TreeNode> childE = nullptr;
    if (tokens[currentToken].getTokenType() == Symbol::tosy){
        childE = terminal(Symbol::tosy);
    } else if (tokens[currentToken].getTokenType() == Symbol::downtosy) {
        childE = terminal(Symbol::downtosy);
    }
    if(!childE){ currentToken = start; currentProd = prevProd; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childF = expressionNode();
    if(!childF){ currentToken = start; currentProd = prevProd; return nullptr;}
    //dosy
    std::shared_ptr<TreeNode> childG = terminal(Symbol::dosy);
    if(!childG){ currentToken = start; currentProd = prevProd; return nullptr;}
    //compound-statement
    std::shared_ptr<TreeNode> childH = compoundStatement();
    if(!childH){ currentToken = start; currentProd = prevProd; return nullptr;}


    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);
    ptr->addChild(childF);
    ptr->addChild(childG);
    ptr->addChild(childH);
    
    currentProd = prevProd; return ptr;
}

// <procedure/function-call> -> ident + (lparent + parameter-list? + rparent)
std::shared_ptr<TreeNode> Parser::procedureFunctionCall(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PROCEDURE_FUNCTION_CALL;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROCEDURE_FUNCTION_CALL);
    //indent
    std::shared_ptr<TreeNode> childA = terminal(Symbol::ident);
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    //(lparent + parameter-list? + rparent)
    std::shared_ptr<TreeNode> childB = terminal(Symbol::lparent);
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);
    if (tokens[currentToken].getTokenType() != Symbol::rparent){ //asumsi parameterList
        std::shared_ptr<TreeNode> childC = parameterList();
        if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childC);
    }
    std::shared_ptr<TreeNode> childD = terminal(Symbol::rparent);
    if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childD);
    
    currentProd = prevProd; return ptr;
}

// <parameter-list> -> expression (comma + expression)*
std::shared_ptr<TreeNode> Parser::parameterList(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::PARAMETER_LIST;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PARAMETER_LIST);
    //expression
    std::shared_ptr<TreeNode> childA = expressionNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    //(comma + expression)*
    while(true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = expressionNode();
            if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
            ptr->addChild(childC);
        } else break;
    }

    currentProd = prevProd; return ptr;
}

// <expression> -> simple-expression (relational-operator + simple-expression)?
std::shared_ptr<TreeNode> Parser::expressionNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::EXPRESSION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::EXPRESSION);
    //simple-expression
    std::shared_ptr<TreeNode> childA = simpleExpression();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    //(relational-operator + simple-expression)?
    std::shared_ptr<TreeNode> childB = relationalOperator();
    if(childB){ 
        ptr->addChild(childB);
        std::shared_ptr<TreeNode> childC = simpleExpression();
        if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childC);
    }
    
    currentProd = prevProd; return ptr;
}

// <simple-expression> -> (plus | minus)? term (additive-operator + term)*
std::shared_ptr<TreeNode> Parser::simpleExpression(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::SIMPLE_EXPRESSION;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::SIMPLE_EXPRESSION);
    //(plus | minus)?
    std::shared_ptr<TreeNode> childA = terminal(Symbol::plus);
    if(childA){ 
        ptr->addChild(childA);
    } else{
        childA = terminal(Symbol::minus);
        if (childA){
            ptr->addChild(childA);
        }
    }
    //term
    std::shared_ptr<TreeNode> childB = termNode();
    if(!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childB);
    //(additive-operator + term)*
    while (true){  
        std::shared_ptr<TreeNode> childC = additiveOperator();
        if(childC){ 
            ptr->addChild(childC);
            std::shared_ptr<TreeNode> childD = termNode();
            if(!childD){ currentToken = start; currentProd = prevProd; return nullptr;}
            ptr->addChild(childD);
        } else break;
    }

    currentProd = prevProd; return ptr;
}

// <term> -> factor (multiplicative-operator + factor)*
std::shared_ptr<TreeNode> Parser::termNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::TERM;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::TERM);
    //factor
    std::shared_ptr<TreeNode> childA = factorNode();
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}
    ptr->addChild(childA);
    //(multiplicative-operator + factor)*
    while(true){ 
        std::shared_ptr<TreeNode> childB = multiplicativeOperator();
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = factorNode();
            if(!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
            ptr->addChild(childC);
        } else break;
    }

    currentProd = prevProd; return ptr;
}

// <factor> -> ident | intcon | realcon | charcon | string | (lparent + expression + rparent) | (notsy + factor) | procedure/function-call | variable
std::shared_ptr<TreeNode> Parser::factorNode(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::FACTOR;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FACTOR);
    //ident | intcon | realcon | charcon | string | (lparent + expression + rparent) | (notsy + factor) | procedure/function-call | variable
    std::shared_ptr<TreeNode> child = nullptr;
    if (tokens[currentToken].getTokenType() == Symbol::ident){
        child = procedureFunctionCall();
        if (!child) child = variableNode();
        if (!child) child = terminal(Symbol::ident);
        if (!child){ currentToken = start; currentProd = prevProd; return nullptr; }
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    }
    child = terminal(Symbol::intcon);
    if (!child) child = terminal(Symbol::realcon);
    if (!child) child = terminal(Symbol::charcon);
    if (!child) child = terminal(Symbol::string);

    if (child){
        ptr->addChild(child);
        currentProd = prevProd; return ptr;
    } 
    //masih blm match
    child = terminal(Symbol::notsy);
    if (child){
        ptr->addChild(child);
        std::shared_ptr<TreeNode> childB = factorNode();
        if (!childB) { currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childB);
        currentProd = prevProd; return ptr;
    }

    child = terminal(Symbol::lparent);
    if (child){
        ptr->addChild(child);
        std::shared_ptr<TreeNode> childB = expressionNode();
        if (!childB){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childB);
        std::shared_ptr<TreeNode> childC = terminal(Symbol::rparent);
        if (!childC){ currentToken = start; currentProd = prevProd; return nullptr;}
        ptr->addChild(childC);
        currentProd = prevProd; return ptr;
    }
    currentToken = start;
    currentProd = prevProd; return nullptr;
}

// <relational-operator> -> eql | neq | gtr | geq | lss | leq
std::shared_ptr<TreeNode> Parser::relationalOperator(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::RELATIONAL_OPERATOR;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::RELATIONAL_OPERATOR);
    //eql | neq | gtr | geq | lss | leq
    std::shared_ptr<TreeNode> childA = nullptr;
    Symbol t = tokens[currentToken].getTokenType();
    if (t == Symbol::eql){
        childA = terminal(Symbol::eql);
    } else if (t == Symbol::neq){
        childA = terminal(Symbol::neq);
    } else if (t == Symbol::gtr){
        childA = terminal(Symbol::gtr);
    } else if (t == Symbol::geq){
        childA = terminal(Symbol::geq);
    } else if (t == Symbol::lss){
        childA = terminal(Symbol::lss);
    } else if (t == Symbol::leq){
        childA = terminal(Symbol::leq);
    }
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);

    currentProd = prevProd; return ptr;
}

// <additive-operator> -> plus | minus | orsy
std::shared_ptr<TreeNode> Parser::additiveOperator(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::ADDITIVE_OPERATOR;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::ADDITIVE_OPERATOR);
    //plus | minus | orsy
    std::shared_ptr<TreeNode> childA = nullptr;
    Symbol t = tokens[currentToken].getTokenType();
    if (t == Symbol::plus){
        childA = terminal(Symbol::plus);
    } else if (t == Symbol::minus){
        childA = terminal(Symbol::minus);
    } else if (t == Symbol::orsy){
        childA = terminal(Symbol::orsy);
    } 
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);

    currentProd = prevProd; return ptr;
}

// <multiplicative-operator> -> times | rdiv | idiv | imod | andsy
std::shared_ptr<TreeNode> Parser::multiplicativeOperator(){
    Symbol prevProd = currentProd;
    currentProd = Symbol::MULTIPLICATIVE_OPERATOR;
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::MULTIPLICATIVE_OPERATOR);
    //times | rdiv | idiv | imod | andsy
    std::shared_ptr<TreeNode> childA = nullptr;
    Symbol t = tokens[currentToken].getTokenType();
    if (t == Symbol::times){
        childA = terminal(Symbol::times);
    } else if (t == Symbol::rdiv){
        childA = terminal(Symbol::rdiv);
    } else if (t == Symbol::idiv){
        childA = terminal(Symbol::idiv);
    } else if (t == Symbol::imod){
        childA = terminal(Symbol::imod);
    } else if (t == Symbol::andsy){
        childA = terminal(Symbol::andsy);
    } 
    if(!childA){ currentToken = start; currentProd = prevProd; return nullptr;}

    ptr->addChild(childA);

    currentProd = prevProd; return ptr;
}
