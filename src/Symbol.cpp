#include "Symbol.hpp"

std::string toString(Symbol s) {
    switch (s) {
        case Symbol::intcon: return "intcon";
        case Symbol::realcon: return "realcon";
        case Symbol::charcon: return "charcon";
        case Symbol::string: return "string";
        case Symbol::notsy: return "notsy";
        case Symbol::plus: return "plus";
        case Symbol::minus: return "minus";
        case Symbol::times: return "times";
        case Symbol::idiv: return "idiv";
        case Symbol::rdiv: return "rdiv";
        case Symbol::imod: return "imod";
        case Symbol::andsy: return "andsy";
        case Symbol::orsy: return "orsy";
        case Symbol::eql: return "eql";
        case Symbol::neq: return "neq";
        case Symbol::gtr: return "gtr";
        case Symbol::geq: return "geq";
        case Symbol::lss: return "lss";
        case Symbol::leq: return "leq";
        case Symbol::lparent: return "lparent";
        case Symbol::rparent: return "rparent";
        case Symbol::lbrack: return "lbrack";
        case Symbol::rbrack: return "rbrack";
        case Symbol::comma: return "comma";
        case Symbol::semicolon: return "semicolon";
        case Symbol::period: return "period";
        case Symbol::colon: return "colon";
        case Symbol::becomes: return "becomes";
        case Symbol::constsy: return "constsy";
        case Symbol::typesy: return "typesy";
        case Symbol::varsy: return "varsy";
        case Symbol::functionsy: return "functionsy";
        case Symbol::proceduresy: return "proceduresy";
        case Symbol::arraysy: return "arraysy";
        case Symbol::recordsy: return "recordsy";
        case Symbol::programsy: return "programsy";
        case Symbol::ident: return "ident";
        case Symbol::beginsy: return "beginsy";
        case Symbol::ifsy: return "ifsy";
        case Symbol::casesy: return "casesy";
        case Symbol::repeatsy: return "repeatsy";
        case Symbol::whilesy: return "whilesy";
        case Symbol::forsy: return "forsy";
        case Symbol::endsy: return "endsy";
        case Symbol::elsesy: return "elsesy";
        case Symbol::untilsy: return "untilsy";
        case Symbol::ofsy: return "ofsy";
        case Symbol::dosy: return "dosy";
        case Symbol::tosy: return "tosy";
        case Symbol::downtosy: return "downtosy";
        case Symbol::thensy: return "thensy";
        case Symbol::comment: return "comment";
        case Symbol::unknown: return "unknown";

        case Symbol::PROGRAM: return "<program>";
        case Symbol::PROGRAM_HEADER: return "<program-header>";
        case Symbol::DECLARATION_PART: return "<declaration-part>";
        case Symbol::CONST_DECLARATION: return "<const-declaration>";
        case Symbol::CONSTANT: return "<constant>";
        case Symbol::TYPE_DECLARATION: return "<type-declaration>";
        case Symbol::VAR_DECLARATION: return "<var-declaration>";
        case Symbol::IDENTIFIER_LIST: return "<identifier-list>";
        case Symbol::TYPE: return "<type>";
        case Symbol::ARRAY_TYPE: return "<array-type>";
        case Symbol::RANGE: return "<range>";
        case Symbol::ENUMERATED: return "<enumerated>";
        case Symbol::RECORD_TYPE: return "<record-type>";
        case Symbol::FIELD_LIST: return "<field-list>";
        case Symbol::FIELD_PART: return "<field-part>";
        case Symbol::SUBPROGRAM_DECLARATION: return "<subprogram-declaration>";
        case Symbol::PROCEDURE_DECLARATION: return "<procedure-declaration>";
        case Symbol::FUNCTION_DECLARATION: return "<function-declaration>";
        case Symbol::BLOCK: return "<block>";
        case Symbol::FORMAL_PARAMETER_LIST: return "<formal-parameter-list>";
        case Symbol::PARAMETER_GROUP: return "<parameter-group>";
        case Symbol::COMPOUND_STATEMENT: return "<compound-statement>";
        case Symbol::STATEMENT_LIST: return "<statement-list>";
        case Symbol::STATEMENT: return "<statement>";
        case Symbol::VARIABLE: return "<variable>";
        case Symbol::COMPONENT_VARIABLE: return "<component-variable>";
        case Symbol::INDEX_LIST: return "<index-list>";
        case Symbol::ASSIGNMENT_STATEMENT: return "<assignment-statement>";
        case Symbol::IF_STATEMENT: return "<if-statement>";
        case Symbol::CASE_STATEMENT: return "<case-statement>";
        case Symbol::CASE_BLOCK: return "<case-block>";
        case Symbol::WHILE_STATEMENT: return "<while-statement>";
        case Symbol::REPEAT_STATEMENT: return "<repeat-statement>";
        case Symbol::FOR_STATEMENT: return "<for-statement>";
        case Symbol::PROCEDURE_FUNCTION_CALL: return "<procedure/function-call>";
        case Symbol::PARAMETER_LIST: return "<parameter-list>";
        case Symbol::EXPRESSION: return "<expression>";
        case Symbol::SIMPLE_EXPRESSION: return "<simple-expression>";
        case Symbol::TERM: return "<term>";
        case Symbol::FACTOR: return "<factor>";
        case Symbol::RELATIONAL_OPERATOR: return "<relational-operator>";
        case Symbol::ADDITIVE_OPERATOR: return "<additive-operator>";
        case Symbol::MULTIPLICATIVE_OPERATOR: return "<multiplicative-operator>";
        default: return "unknown";
    }
}

bool isToken(Symbol s){
    return s < Symbol::PROGRAM || s == Symbol::unknown;
}