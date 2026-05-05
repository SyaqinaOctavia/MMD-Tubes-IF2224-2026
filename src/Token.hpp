#pragma once
#include <string>
#include <map>
#include "Symbol.hpp"

class Token {
    private:
        Symbol tokenType;
        std::string value;
    public:
        static const std::map<std::string, Symbol> keywordMap;
        Token(Symbol symbol) : tokenType(symbol){}
        Token(Symbol symbol, std::string value) : tokenType(symbol), value(value){}
        Symbol getTokenType() const { return tokenType; }
        std::string getValue() const { return value; }
        std::string toString() const;
};

inline const std::map<std::string, Symbol> Token::keywordMap = {
    {"intcon", Symbol::intcon},
    {"realcon", Symbol::realcon},
    {"charcon", Symbol::charcon},
    {"string", Symbol::string},
    {"notsy", Symbol::notsy},
    {"plus", Symbol::plus},
    {"minus", Symbol::minus},
    {"times", Symbol::times},
    {"idiv", Symbol::idiv},
    {"rdiv", Symbol::rdiv},
    {"imod", Symbol::imod},
    {"andsy", Symbol::andsy},
    {"orsy", Symbol::orsy},
    {"eql", Symbol::eql},
    {"neq", Symbol::neq},
    {"gtr", Symbol::gtr},
    {"geq", Symbol::geq},
    {"lss", Symbol::lss},
    {"leq", Symbol::leq},
    {"lparent", Symbol::lparent},
    {"rparent", Symbol::rparent},
    {"lbrack", Symbol::lbrack},
    {"rbrack", Symbol::rbrack},
    {"comma", Symbol::comma},
    {"semicolon", Symbol::semicolon},
    {"period", Symbol::period},
    {"colon", Symbol::colon},
    {"becomes", Symbol::becomes},
    {"constsy", Symbol::constsy},
    {"typesy", Symbol::typesy},
    {"varsy", Symbol::varsy},
    {"functionsy", Symbol::functionsy},
    {"proceduresy", Symbol::proceduresy},
    {"arraysy", Symbol::arraysy},
    {"recordsy", Symbol::recordsy},
    {"programsy", Symbol::programsy},
    {"ident", Symbol::ident},
    {"beginsy", Symbol::beginsy},
    {"ifsy", Symbol::ifsy},
    {"casesy", Symbol::casesy},
    {"repeatsy", Symbol::repeatsy},
    {"whilesy", Symbol::whilesy},
    {"forsy", Symbol::forsy},
    {"endsy", Symbol::endsy},
    {"elsesy", Symbol::elsesy},
    {"untilsy", Symbol::untilsy},
    {"ofsy", Symbol::ofsy},
    {"dosy", Symbol::dosy},
    {"tosy", Symbol::tosy},
    {"downtosy", Symbol::downtosy},
    {"thensy", Symbol::thensy},
    {"comment", Symbol::comment}
};