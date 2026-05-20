#pragma once

#include <vector>
#include <memory>
#include "../Symbol.hpp"

enum class ASTType {
    // Types
    SimpleTypeNode,
    ArrayTypeNode,
    EnumTypeNode,
    RangeTypeNode,
    FieldTypeNode,
    // Declarations
    ConstDeclNode,
    TypeDeclNode,
    VarDeclNode,
    ParamDeclNode,
    ProcDeclNode,
    FuncDeclNode,
    // Expressions
    LiteralNode,
    CallNode,
    BinaryOpNode,
    UnaryOpNode,
    VarRefNode,
    ArrayAccessNode,
    FieldAccessNode,
    // Statements
    AssignNode,
    IfNode,
    WhileNode,
    ForNode,
    RepeatNode,
    CaseNode,
    CompoundNode,
    // Root
    ProgramNode
};

enum class LiteralKind { Int, Real, Bool, Char, String };

class ASTNode {
protected:
    ASTType nodeType;
};

class ExprNode : public ASTNode {};

class LiteralNode : public ExprNode {
private:
    LiteralKind kind;
    std::string value;
public:
    LiteralNode(LiteralKind kind, std::string value) : kind(kind), value(value) { nodeType = ASTType::LiteralNode; }
    LiteralKind getKind(){ return kind; }
    std::string getValue(){ return value; }
};

class TypeNode : public ASTNode {
public:
    enum class Kind { Simple, Array, Record, Range, Enumerated };
    TypeNode(Kind kind) : kind(kind) {}
    Kind getKind() const { return kind; }
private:
    Kind kind;
};

class SimpleTypeNode : public TypeNode {
public:
    SimpleTypeNode(std::string name) : TypeNode(Kind::Simple), name(name) { nodeType = ASTType::SimpleTypeNode; }
    std::string getName() const { return name; }
private:
    std::string name;
};

class ArrayTypeNode : public TypeNode {
public:
    ArrayTypeNode(std::shared_ptr<TypeNode> type, std::shared_ptr<TypeNode> index)
        : TypeNode(Kind::Array), type(type), index(index) { nodeType = ASTType::ArrayTypeNode; }
    std::shared_ptr<TypeNode> getType() const { return type; }
private:
    std::shared_ptr<TypeNode> type;
    std::shared_ptr<TypeNode> index; // harus simpletype atau rangetype
};

class EnumTypeNode : public TypeNode {
public:
    EnumTypeNode(std::vector<std::string> enum_values) : TypeNode(Kind::Enumerated), enum_values(enum_values) { nodeType = ASTType::EnumTypeNode; }
    std::vector<std::string> getEnumValues() const { return enum_values; }
    void pushEnumValue(std::string value) { enum_values.push_back(value); }
private:
    std::vector<std::string> enum_values;
};

class RangeTypeNode : public TypeNode {
public:
    RangeTypeNode(std::shared_ptr<ExprNode> low, std::shared_ptr<ExprNode> high) : TypeNode(Kind::Range), low(low), high(high) { nodeType = ASTType::RangeTypeNode; }
    std::shared_ptr<ExprNode> getLow() const { return low; }
    std::shared_ptr<ExprNode> getHigh() const { return high; }
private:
    std::shared_ptr<ExprNode> low;
    std::shared_ptr<ExprNode> high;
};

class FieldTypeNode : public TypeNode {
public:
    FieldTypeNode(std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> fields)
        : TypeNode(Kind::Record), fields(fields) { nodeType = ASTType::FieldTypeNode; }
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> getFields() const { return fields; }
    void pushField(std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>> field) { fields.push_back(field); }
private:
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> fields;
};

class StmtNode : public ASTNode {};

class DeclNode : public ASTNode {
public:
    enum class Kind { Const, Type, Var, Proc, Func, Param };
    DeclNode(Kind kind) : kind(kind) {}
    Kind getKind() const { return kind; }
private:
    Kind kind;
};

class TypeDeclNode : public DeclNode {
public:
    TypeDeclNode(std::string name, std::shared_ptr<TypeNode> type_spec)
        : DeclNode(Kind::Type), name(name), type_spec(type_spec) { nodeType = ASTType::TypeDeclNode; }
    std::string getName() const { return name; }
    std::shared_ptr<TypeNode> getTypeSpec() const { return type_spec; }
private:
    std::string name;
    std::shared_ptr<TypeNode> type_spec;
};

class ConstDeclNode : public DeclNode {
public:
    ConstDeclNode(std::string name, std::shared_ptr<ExprNode> value)
        : DeclNode(Kind::Const), name(name), value(value) { nodeType = ASTType::ConstDeclNode; }
    std::string getName() const { return name; }
    std::shared_ptr<ExprNode> getValue() const { return value; }
private:
    std::string name;
    std::shared_ptr<ExprNode> value;
};

class VarDeclNode : public DeclNode {
public:
    VarDeclNode(std::vector<std::string> names, std::shared_ptr<TypeNode> type)
        : DeclNode(Kind::Var), names(names), type(type) { nodeType = ASTType::VarDeclNode; }
    std::vector<std::string> getNames() const { return names; }
    std::shared_ptr<TypeNode> getType() const { return type; }
    void pushName(std::string name) { names.push_back(name); }
private:
    std::vector<std::string> names;
    std::shared_ptr<TypeNode> type;
};

class ParamDeclNode : public DeclNode {
public:
    ParamDeclNode(std::vector<std::string> names, std::shared_ptr<TypeNode> type, bool is_var_param = false)
        : DeclNode(Kind::Param), names(names), type(type), is_var_param(is_var_param) { nodeType = ASTType::ParamDeclNode; }
    std::vector<std::string> getNames() const { return names; }
    std::shared_ptr<TypeNode> getType() const { return type; }
    bool isVarParam() const { return is_var_param; }
    void pushName(std::string name) { names.push_back(name); }
private:
    std::vector<std::string> names;
    std::shared_ptr<TypeNode> type;
    bool is_var_param;
};

class ProcDeclNode : public DeclNode {
public:
    ProcDeclNode(std::string name, std::vector<std::shared_ptr<ParamDeclNode>> params,
                 std::vector<std::shared_ptr<DeclNode>> local_var, std::shared_ptr<StmtNode> body)
        : DeclNode(Kind::Proc), name(name), params(params), local_var(local_var), body(body) { nodeType = ASTType::ProcDeclNode; }
    std::string getName() const { return name; }
    std::vector<std::shared_ptr<ParamDeclNode>> getParams() const { return params; }
    std::vector<std::shared_ptr<DeclNode>> getLocalVar() const { return local_var; }
    std::shared_ptr<StmtNode> getBody() const { return body; }
    void pushParam(std::shared_ptr<ParamDeclNode> param) { params.push_back(param); }
    void pushLocalVar(std::shared_ptr<DeclNode> var) { local_var.push_back(var); }
private:
    std::string name;
    std::vector<std::shared_ptr<ParamDeclNode>> params;
    std::vector<std::shared_ptr<DeclNode>> local_var;
    std::shared_ptr<StmtNode> body;
};

class FuncDeclNode : public DeclNode {
public:
    FuncDeclNode(std::string name, std::shared_ptr<TypeNode> return_type,
                 std::vector<std::shared_ptr<ParamDeclNode>> params,
                 std::vector<std::shared_ptr<DeclNode>> local_var, std::shared_ptr<StmtNode> body)
        : DeclNode(Kind::Func), name(name), return_type(return_type), params(params), local_var(local_var), body(body) { nodeType = ASTType::FuncDeclNode; }
    std::string getName() const { return name; }
    std::shared_ptr<TypeNode> getReturnType() const { return return_type; }
    std::vector<std::shared_ptr<ParamDeclNode>> getParams() const { return params; }
    std::vector<std::shared_ptr<DeclNode>> getLocalVar() const { return local_var; }
    std::shared_ptr<StmtNode> getBody() const { return body; }
    void pushParam(std::shared_ptr<ParamDeclNode> param) { params.push_back(param); }
    void pushLocalVar(std::shared_ptr<DeclNode> var) { local_var.push_back(var); }
private:
    std::string name;
    std::shared_ptr<TypeNode> return_type;
    std::vector<std::shared_ptr<ParamDeclNode>> params;
    std::vector<std::shared_ptr<DeclNode>> local_var;
    std::shared_ptr<StmtNode> body;
};

class CallNode : public ExprNode {
public:
    CallNode(std::string name, std::vector<std::shared_ptr<ExprNode>> args)
        : name(name), args(args) { nodeType = ASTType::CallNode; }
    std::string getName() const { return name; }
    std::vector<std::shared_ptr<ExprNode>> getArgs() const { return args; }
    void pushArg(std::shared_ptr<ExprNode> arg) { args.push_back(arg); }
private:
    std::string name;
    std::vector<std::shared_ptr<ExprNode>> args;
};

class BinaryOpNode : public ExprNode {
public:
    BinaryOpNode(std::string op, std::shared_ptr<ExprNode> left, std::shared_ptr<ExprNode> right)
        : op(op), left(left), right(right) { nodeType = ASTType::BinaryOpNode; }
    std::string getOp() const { return op; }
    std::shared_ptr<ExprNode> getLeft() const { return left; }
    std::shared_ptr<ExprNode> getRight() const { return right; }
private:
    std::string op;
    std::shared_ptr<ExprNode> left;
    std::shared_ptr<ExprNode> right;
};

class UnaryOpNode : public ExprNode {
public:
    UnaryOpNode(std::string op, std::shared_ptr<ExprNode> operand)
        : op(op), operand(operand) { nodeType = ASTType::UnaryOpNode; }
    std::string getOp() const { return op; }
    std::shared_ptr<ExprNode> getOperand() const { return operand; }
private:
    std::string op;
    std::shared_ptr<ExprNode> operand;
};

class VarRefNode : public ExprNode {
public:
    VarRefNode(std::string name) : name(name) { nodeType = ASTType::VarRefNode; }
    std::string getName() const { return name; }
private:
    std::string name;
};

class ArrayAccessNode : public ExprNode {
public:
    ArrayAccessNode(std::shared_ptr<ExprNode> array, std::shared_ptr<ExprNode> index)
        : array(array), index(index) { nodeType = ASTType::ArrayAccessNode; }
    std::shared_ptr<ExprNode> getArray() const { return array; }
    std::shared_ptr<ExprNode> getIndex() const { return index; }
private:
    std::shared_ptr<ExprNode> array;
    std::shared_ptr<ExprNode> index;
};

class FieldAccessNode : public ExprNode {
public:
    FieldAccessNode(std::shared_ptr<ExprNode> record, std::string field_name)
        : record(record), field_name(field_name) { nodeType = ASTType::FieldAccessNode; }
    std::shared_ptr<ExprNode> getRecord() const { return record; }
    std::string getFieldName() const { return field_name; }
private:
    std::shared_ptr<ExprNode> record;
    std::string field_name;
};

class IfNode : public StmtNode {
public:
    IfNode(std::shared_ptr<ExprNode> condition, std::shared_ptr<StmtNode> thenblock,
           std::shared_ptr<StmtNode> elseblock = nullptr)
        : condition(condition), thenblock(thenblock), elseblock(elseblock) { nodeType = ASTType::IfNode; }
    std::shared_ptr<ExprNode> getCondition() const { return condition; }
    std::shared_ptr<StmtNode> getThenBlock() const { return thenblock; }
    std::shared_ptr<StmtNode> getElseBlock() const { return elseblock; }
private:
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<StmtNode> thenblock;
    std::shared_ptr<StmtNode> elseblock;
};

class WhileNode : public StmtNode {
public:
    WhileNode(std::shared_ptr<ExprNode> condition, std::shared_ptr<StmtNode> body)
        : condition(condition), body(body) { nodeType = ASTType::WhileNode; }
    std::shared_ptr<ExprNode> getCondition() const { return condition; }
    std::shared_ptr<StmtNode> getBody() const { return body; }
private:
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<StmtNode> body;
};

class ForNode : public StmtNode {
public:
    ForNode(bool goes_up, std::string movingvar, std::shared_ptr<ExprNode> startpoint,
            std::shared_ptr<ExprNode> endpoint, std::shared_ptr<StmtNode> body)
        : goes_up(goes_up), movingvar(movingvar), startpoint(startpoint), endpoint(endpoint), body(body) { nodeType = ASTType::ForNode; }
    bool goesUp() const { return goes_up; }
    std::string getMovingVar() const { return movingvar; }
    std::shared_ptr<ExprNode> getStartPoint() const { return startpoint; }
    std::shared_ptr<ExprNode> getEndPoint() const { return endpoint; }
    std::shared_ptr<StmtNode> getBody() const { return body; }
private:
    bool goes_up;
    std::string movingvar;
    std::shared_ptr<ExprNode> startpoint;
    std::shared_ptr<ExprNode> endpoint;
    std::shared_ptr<StmtNode> body;
};

class RepeatNode : public StmtNode {
public:
    RepeatNode(std::shared_ptr<StmtNode> body, std::shared_ptr<ExprNode> untilcondition)
        : body(body), untilcondition(untilcondition) { nodeType = ASTType::RepeatNode; }
    std::shared_ptr<StmtNode> getBody() const { return body; }
    std::shared_ptr<ExprNode> getUntilCondition() const { return untilcondition; }
private:
    std::shared_ptr<StmtNode> body;
    std::shared_ptr<ExprNode> untilcondition;
};

class CaseNode : public StmtNode {
public:
    CaseNode(std::shared_ptr<ExprNode> key,
             std::vector<std::pair<std::vector<std::shared_ptr<ExprNode>>, std::shared_ptr<StmtNode>>> cases)
        : key(key), cases(cases) { nodeType = ASTType::CaseNode; }
    std::shared_ptr<ExprNode> getKey() const { return key; }
    std::vector<std::pair<std::vector<std::shared_ptr<ExprNode>>, std::shared_ptr<StmtNode>>> getCases() const { return cases; }
    void pushCase(std::pair<std::vector<std::shared_ptr<ExprNode>>, std::shared_ptr<StmtNode>> case_pair) { cases.push_back(case_pair); }
private:
    std::shared_ptr<ExprNode> key;
    std::vector<std::pair<std::vector<std::shared_ptr<ExprNode>>, std::shared_ptr<StmtNode>>> cases;
};

class AssignNode : public StmtNode {
public:
    AssignNode(std::shared_ptr<ExprNode> target, std::shared_ptr<ExprNode> value)
        : target(target), value(value) { nodeType = ASTType::AssignNode; }
    std::shared_ptr<ExprNode> getTarget() const { return target; }
    std::shared_ptr<ExprNode> getValue() const { return value; }
private:
    std::shared_ptr<ExprNode> target;
    std::shared_ptr<ExprNode> value;
};

class CompoundNode : public StmtNode {
public:
    CompoundNode(std::vector<std::shared_ptr<StmtNode>> statements)
        : statements(statements) { nodeType = ASTType::CompoundNode; }
    std::vector<std::shared_ptr<StmtNode>> getStatements() const { return statements; }
    void pushStatement(std::shared_ptr<StmtNode> stmt) { statements.push_back(stmt); }
private:
    std::vector<std::shared_ptr<StmtNode>> statements;
};

class ProgramNode : public ASTNode {
public:
    ProgramNode(std::string name, std::vector<std::shared_ptr<DeclNode>> declarations,
                std::shared_ptr<CompoundNode> main)
        : name(name), declarations(declarations), main(main) { nodeType = ASTType::ProgramNode; }
    std::string getName() const { return name; }
    std::vector<std::shared_ptr<DeclNode>> getDeclarations() const { return declarations; }
    std::shared_ptr<CompoundNode> getMain() const { return main; }
    void pushDeclaration(std::shared_ptr<DeclNode> decl) { declarations.push_back(decl); }
private:
    std::string name;
    std::vector<std::shared_ptr<DeclNode>> declarations;
    std::shared_ptr<CompoundNode> main;
};

std::shared_ptr<LiteralNode> constantToLiteral(std::shared_ptr<TreeNode> node);
std::vector<std::string> enumToStrings(std::shared_ptr<TreeNode> node);
std::vector<std::string> identifierListToStrings(std::shared_ptr<TreeNode> node);