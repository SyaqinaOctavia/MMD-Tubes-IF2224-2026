#pragma once

#include <vector>
#include <memory>
#include "../Symbol.hpp"

enum class ASTType {
    // Literals
    LiteralNode,
    // Types
    SimpleTypeNode,
    ArrayTypeNode,
    EnumTypeNode,
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
    ASTType NodeType;
};

class ExprNode : public ASTNode {};

class LiteralNode : public ExprNode {
private:
    LiteralKind kind;
    std::string value;
public:
    LiteralNode(LiteralKind kind, std::string value) : kind(kind), value(value) {}
    LiteralKind getKind(){ return kind; }
    std::string getValue(){ return value; }
};

class TypeNode : public ASTNode {
public:
    enum class Kind { Simple, Array, Record, Range, Enumerated };
    Kind kind;
};

class SimpleTypeNode : public TypeNode {
public:
    std::string name;
};

class ArrayTypeNode : public TypeNode {
public:
    std::shared_ptr<TypeNode> type;
    std::shared_ptr<ExprNode> low, high;
};

class EnumTypeNode : public TypeNode {
public:
    std::vector<std::string> enum_values;
};

class FieldTypeNode : public TypeNode {
public:
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> fields;
};

class StmtNode : public ASTNode {};

class DeclNode : public ASTNode {
public:
    enum class Kind { Const, Type, Var, Proc, Func, Param };
    Kind kind;
};

class TypeDeclNode : public DeclNode {
public:
    std::string name;
    std::shared_ptr<TypeNode> type_spec;       // Var, Param, Func return
};

class ConstDeclNode : public DeclNode {
public:
    std::string name;
    std::shared_ptr<ExprNode> value;           // Const
};

class VarDeclNode : public DeclNode {
public:
    std::vector<std::string> names;
    std::shared_ptr<TypeNode> type;
};

class ParamDeclNode : public DeclNode {
public:
    std::vector<std::string> names;
    std::shared_ptr<TypeNode> type;
    bool is_var_param = false;
};

class ProcDeclNode : public DeclNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ParamDeclNode>> params;      // Proc/Func
    std::vector<std::shared_ptr<DeclNode>> local_var; // Proc/Func
    std::shared_ptr<StmtNode> body;            // Proc/Func
};

class FuncDeclNode : public DeclNode {
public:
    std::string name;
    std::shared_ptr<TypeNode> return_type;
    std::vector<std::shared_ptr<ParamDeclNode>> params;      // Proc/Func
    std::vector<std::shared_ptr<DeclNode>> local_var; // Proc/Func
    std::shared_ptr<StmtNode> body;            // Proc/Func
};

class CallNode : public ExprNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ExprNode>> args;
};

class BinaryOpNode : public ExprNode {
public:
    std::string op;
    std::shared_ptr<ExprNode> left;
    std::shared_ptr<ExprNode> right;
};

class UnaryOpNode : public ExprNode {
public:
    std::string op;
    std::shared_ptr<ExprNode> operand;
};

class VarRefNode : public ExprNode {
public:
    std::string name;
};

class ArrayAccessNode : public ExprNode {
public:
    std::shared_ptr<ExprNode> array;
    std::shared_ptr<ExprNode> index;
};

class FieldAccessNode : public ExprNode {
public:
    std::shared_ptr<ExprNode> record;
    std::string field_name;
};

class IfNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<StmtNode> thenblock;
    std::shared_ptr<StmtNode> elseblock;
};

class WhileNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> condition;
    std::shared_ptr<StmtNode> body;
};

class ForNode : public StmtNode {
public:
    bool goes_up;
    std::string movingvar;
    std::shared_ptr<ExprNode> startpoint;
    std::shared_ptr<ExprNode> endpoint;
    std::shared_ptr<StmtNode> body;
};

class RepeatNode : public StmtNode {
public:
    std::shared_ptr<StmtNode> body;
    std::shared_ptr<ExprNode> untilcondition;
};

class CaseNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> key;
    std::vector<std::pair<
        std::vector<std::shared_ptr<ExprNode>>,
        std::shared_ptr<StmtNode>
    >> cases;
};

class AssignNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> target;
    std::shared_ptr<ExprNode> value;
};

class CompoundNode : public StmtNode {
public:
    std::vector<std::shared_ptr<StmtNode>> statements;
};

class ProgramNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<DeclNode>> declarations;
    std::shared_ptr<CompoundNode> main;
};