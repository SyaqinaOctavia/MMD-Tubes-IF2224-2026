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

std::shared_ptr<TreeNode> Parser::variableNode(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::componentVariable(){
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::indexList(){
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
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::CASE_BLOCK);
    //constant
    std::shared_ptr<TreeNode> childA = constantNode();
    if(!childA){ currentToken = start; return nullptr;}
    ptr->addChild(childA);
    //(comma + constant)*
    while (true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = constantNode();
            if(!childC){ currentToken = start; return nullptr;}
            ptr->addChild(childC);
        } else break;  
    }
    //colon
    std::shared_ptr<TreeNode> childD = terminal(Symbol::colon);
    if(!childD){ currentToken = start; return nullptr;}
    ptr->addChild(childD);
    //statement
    std::shared_ptr<TreeNode> childE = statementNode();
    if(!childE){ currentToken = start; return nullptr;}
    ptr->addChild(childE);
    //(semicolon + case-block?)*
    while (true){
        std::shared_ptr<TreeNode> childF = terminal(Symbol::semicolon);
        if(childF){ 
            ptr->addChild(childF);

            auto isFirstCaseBlock = [&](int pos) {
                if (pos >= (int)tokens.size()) return false;
                Symbol s = tokens[pos].getTokenType();
                if (s == Symbol::charcon || s == Symbol::string ||
                    s == Symbol::ident || s == Symbol::intcon ||
                    s == Symbol::realcon)
                    return true;
                if ((s == Symbol::plus || s == Symbol::minus) &&
                    pos + 1 < (int)tokens.size()) {
                    Symbol next_s = tokens[pos + 1].getTokenType();
                    return next_s == Symbol::ident ||
                        next_s == Symbol::intcon ||
                        next_s == Symbol::realcon;
                }
                return false;
            };

            if(isFirstCaseBlock(currentToken)){
                std::shared_ptr<TreeNode> childG = caseBlock();
                if(!childG){ currentToken = start; return nullptr; }
                ptr->addChild(childG);
            } else break;
            
        } else break;
        
    } 
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::whileStatement(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::WHILE_STATEMENT);
    //whilesy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::whilesy);
    if(!childA){ currentToken = start; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childB = expressionNode();
    if(!childB){ currentToken = start; return nullptr;}
    //dosy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::dosy);
    if(!childC){ currentToken = start; return nullptr;}
    //statement
    std::shared_ptr<TreeNode> childD = statementNode();
    if(!childD){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);

    return ptr;
}

std::shared_ptr<TreeNode> Parser::repeatStatement(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::REPEAT_STATEMENT);
    //repeatsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::repeatsy);
    if(!childA){ currentToken = start; return nullptr;}
    //statement-list
    std::shared_ptr<TreeNode> childB = statementList();
    if(!childB){ currentToken = start; return nullptr;}
    //untilsy
    std::shared_ptr<TreeNode> childC = terminal(Symbol::untilsy);
    if(!childC){ currentToken = start; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childD = expressionNode();
    if(!childD){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::forStatement(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FOR_STATEMENT);
    //forsy
    std::shared_ptr<TreeNode> childA = terminal(Symbol::forsy);
    if(!childA){ currentToken = start; return nullptr;}
    //ident
    std::shared_ptr<TreeNode> childB = terminal(Symbol::ident);
    if(!childB){ currentToken = start; return nullptr;}
    //becomes
    std::shared_ptr<TreeNode> childC = terminal(Symbol::becomes);
    if(!childC){ currentToken = start; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childD = expressionNode();
    if(!childD){ currentToken = start; return nullptr;}
    //( tosy | downtosy)
    std::shared_ptr<TreeNode> childE = nullptr;
    if (tokens[currentToken].getTokenType() == Symbol::tosy){
        childE = terminal(Symbol::tosy);
    } else if (tokens[currentToken].getTokenType() == Symbol::downtosy) {
        childE = terminal(Symbol::downtosy);
    }
    if(!childE){ currentToken = start; return nullptr;}
    //expression
    std::shared_ptr<TreeNode> childF = expressionNode();
    if(!childF){ currentToken = start; return nullptr;}
    //dosy
    std::shared_ptr<TreeNode> childG = terminal(Symbol::dosy);
    if(!childG){ currentToken = start; return nullptr;}
    //statement
    std::shared_ptr<TreeNode> childH = statementNode();
    if(!childH){ currentToken = start; return nullptr;}

    ptr->addChild(childA);
    ptr->addChild(childB);
    ptr->addChild(childC);
    ptr->addChild(childD);
    ptr->addChild(childE);
    ptr->addChild(childF);
    ptr->addChild(childG);
    ptr->addChild(childH);
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::procedureFunctionCall(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PROCEDURE_FUNCTION_CALL);
    //indent
    std::shared_ptr<TreeNode> childA = terminal(Symbol::ident);
    if(!childA){ currentToken = start; return nullptr;}
    ptr->addChild(childA);
    //(lparent + parameter-list? + rparent)
    std::shared_ptr<TreeNode> childB = terminal(Symbol::lparent);
    if(!childB){ currentToken = start; return nullptr;}
    ptr->addChild(childB);
    if (tokens[currentToken].getTokenType() != Symbol::rparent){ //asumsi parameterList
        std::shared_ptr<TreeNode> childC = parameterList();
        if(!childC){ currentToken = start; return nullptr;}
        ptr->addChild(childC);
    }
    std::shared_ptr<TreeNode> childD = terminal(Symbol::rparent);
    if(!childD){ currentToken = start; return nullptr;}
    ptr->addChild(childD);
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::parameterList(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::PARAMETER_LIST);
    //expression
    std::shared_ptr<TreeNode> childA = expressionNode();
    if(!childA){ currentToken = start; return nullptr;}
    ptr->addChild(childA);
    //(comma + expression)*
    while(true){
        std::shared_ptr<TreeNode> childB = terminal(Symbol::comma);
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = expressionNode();
            if(!childC){ currentToken = start; return nullptr;}
            ptr->addChild(childC);
        } else break;
    }

    return ptr;
}

std::shared_ptr<TreeNode> Parser::expressionNode(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::EXPRESSION);
    //simple-expression
    std::shared_ptr<TreeNode> childA = simpleExpression();
    if(!childA){ currentToken = start; return nullptr;}
    ptr->addChild(childA);
    //(relational-operator + simple-expression)?
    std::shared_ptr<TreeNode> childB = relationalOperator();
    if(childB){ 
        ptr->addChild(childB);
        std::shared_ptr<TreeNode> childC = simpleExpression();
        if(!childC){ currentToken = start; return nullptr;}
        ptr->addChild(childC);
    }
    
    return ptr;
}

std::shared_ptr<TreeNode> Parser::simpleExpression(){
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
    if(!childB){ currentToken = start; return nullptr;}
    ptr->addChild(childB);
    //(additive-operator + term)*
    while (true){  
        std::shared_ptr<TreeNode> childC = additiveOperator();
        if(childC){ 
            ptr->addChild(childC);
            std::shared_ptr<TreeNode> childD = termNode();
            if(!childD){ currentToken = start; return nullptr;}
            ptr->addChild(childD);
        } else break;
    }

    return ptr;
}

std::shared_ptr<TreeNode> Parser::termNode(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::TERM);
    //factor
    std::shared_ptr<TreeNode> childA = factorNode();
    if(!childA){ currentToken = start; return nullptr;}
    ptr->addChild(childA);
    //(multiplicative-operator + factor)*
    while(true){ 
        std::shared_ptr<TreeNode> childB = multiplicativeOperator();
        if(childB){ 
            ptr->addChild(childB);
            std::shared_ptr<TreeNode> childC = factorNode();
            if(!childC){ currentToken = start; return nullptr;}
            ptr->addChild(childC);
        } else break;
    }

    return ptr;
}

std::shared_ptr<TreeNode> Parser::factorNode(){
    int start = currentToken;
    std::shared_ptr<TreeNode> ptr = std::make_shared<TreeNode>(Symbol::FACTOR);
    //ident | intcon | realcon | charcon | string | (lparent + expression + rparent) | (notsy + factor) | procedure/function-call | variable
    std::shared_ptr<TreeNode> child = nullptr;
    if (tokens[currentToken].getTokenType() == Symbol::ident){
        child = procedureFunctionCall();
        if (!child) child = variableNode();
        if (!child) child = terminal(Symbol::ident);
        if (!child){ currentToken = start; return nullptr; }
        ptr->addChild(child);
        return ptr;
    }
    child = terminal(Symbol::intcon);
    if (!child) child = terminal(Symbol::realcon);
    if (!child) child = terminal(Symbol::charcon);
    if (!child) child = terminal(Symbol::string);

    if (child){
        ptr->addChild(child);
        return ptr;
    } 
    //masih blm match
    child = terminal(Symbol::notsy);
    if (child){
        ptr->addChild(child);
        std::shared_ptr<TreeNode> childB = factorNode();
        if (!childB) { currentToken = start; return nullptr;}
        ptr->addChild(childB);
        return ptr;
    }

    child = terminal(Symbol::lparent);
    if (child){
        ptr->addChild(child);
        std::shared_ptr<TreeNode> childB = expressionNode();
        if (!childB){ currentToken = start; return nullptr;}
        ptr->addChild(childB);
        std::shared_ptr<TreeNode> childC = terminal(Symbol::rparent);
        if (!childC){ currentToken = start; return nullptr;}
        ptr->addChild(childC);
        return ptr;
    }
    currentToken = start;
    return nullptr;
}

std::shared_ptr<TreeNode> Parser::relationalOperator(){
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
    if(!childA){ currentToken = start; return nullptr;}

    ptr->addChild(childA);

    return ptr;
}

std::shared_ptr<TreeNode> Parser::additiveOperator(){
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
    if(!childA){ currentToken = start; return nullptr;}

    ptr->addChild(childA);

    return ptr;
}

std::shared_ptr<TreeNode> Parser::multiplicativeOperator(){
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
    if(!childA){ currentToken = start; return nullptr;}

    ptr->addChild(childA);

    return ptr;
}
