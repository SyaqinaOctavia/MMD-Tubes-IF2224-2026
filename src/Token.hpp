#pragma once
#include <string>

enum class TokenType {
    intcon,
    realcon,
    charcon,
    string,
    notsy,
    plus,
    minus,
    times,
    idiv,
    rdiv,
    imod,
    andsy,
    orsy,
    eql,
    neq,
    gtr,
    geq,
    lss,
    leq,
    lparent,
    rparent,
    lbrack,
    rbrack,
    comma,
    semicolon,
    period,
    colon,
    becomes,
    constsy,
    typesy,
    varsy,
    functionsy,
    proceduresy,
    arraysy,
    recordsy,
    programsy,
    ident,
    beginsy,
    ifsy,
    casesy,
    repeatsy,
    whilesy,
    forsy,
    endsy,
    elsesy,
    untilsy,
    ofsy,
    dosy,
    tosy,
    downtosy,
    thensy,
    comment,
};

class Token {
    private:
        TokenType tokenType;
        std::string value;
    public:
        TokenType getTokenType() const { return tokenType; }
        std::string getValue() const { return value; }
};