#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Token.hpp"
#include "ParseTree.hpp"

class Parser {
    private:
        std::vector<Token> tokens;
        int currentToken;
        Token *received;
        Symbol expected;
    public:
        Parser(std::vector<Token> tokens) : tokens(tokens), currentToken(0) {}
        std::shared_ptr<TreeNode> buildTree();
        std::shared_ptr<TreeNode> terminal(Symbol symbol);

        std::shared_ptr<TreeNode> programNode();
        std::shared_ptr<TreeNode> programHeader();
        std::shared_ptr<TreeNode> declarationPart();
        std::shared_ptr<TreeNode> constDeclaration();
        std::shared_ptr<TreeNode> constantNode();
        std::shared_ptr<TreeNode> typeDeclaration();
        std::shared_ptr<TreeNode> varDeclaration();
        std::shared_ptr<TreeNode> identifierList();
        std::shared_ptr<TreeNode> typeNode();
        std::shared_ptr<TreeNode> arrayType();
        std::shared_ptr<TreeNode> rangeNode();
        std::shared_ptr<TreeNode> enumerated();
        std::shared_ptr<TreeNode> recordType();
        std::shared_ptr<TreeNode> fieldList();
        std::shared_ptr<TreeNode> fieldPart();
        std::shared_ptr<TreeNode> subprogramDeclaration();
        std::shared_ptr<TreeNode> procedureDeclaration();
        std::shared_ptr<TreeNode> functionDeclaration();
        std::shared_ptr<TreeNode> blockNode();
        std::shared_ptr<TreeNode> formalParameterList();
        std::shared_ptr<TreeNode> parameterGroup();
        std::shared_ptr<TreeNode> compoundStatement();
        std::shared_ptr<TreeNode> statementList();
        std::shared_ptr<TreeNode> statementNode();
        std::shared_ptr<TreeNode> variableNode();
        std::shared_ptr<TreeNode> componentVariable();
        std::shared_ptr<TreeNode> indexList();
        std::shared_ptr<TreeNode> assignmentStatement();
        std::shared_ptr<TreeNode> ifStatement();
        std::shared_ptr<TreeNode> caseStatement();
        std::shared_ptr<TreeNode> caseBlock();
        std::shared_ptr<TreeNode> whileStatement();
        std::shared_ptr<TreeNode> repeatStatement();
        std::shared_ptr<TreeNode> forStatement();
        std::shared_ptr<TreeNode> procedureFunctionCall();
        std::shared_ptr<TreeNode> parameterList();
        std::shared_ptr<TreeNode> expressionNode();
        std::shared_ptr<TreeNode> simpleExpression();
        std::shared_ptr<TreeNode> termNode();
        std::shared_ptr<TreeNode> factorNode();
        std::shared_ptr<TreeNode> relationalOperator();
        std::shared_ptr<TreeNode> additiveOperator();
        std::shared_ptr<TreeNode> multiplicativeOperator();
};