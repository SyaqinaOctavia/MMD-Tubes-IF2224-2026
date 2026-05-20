#pragma once

#include <vector>
#include <memory>
#include "../Symbol.hpp"

enum class ASTType {
    LiteralNode, 
    TypeNode, 
    DeclNode, 
    CallNode, 
    BinaryOpNode, 
    UnaryOpNode, 
    VarRefNode, 
    ArrayAccessNode, 
    FieldAccessNode,
    IfNode, 
    WhileNode, 
    ForNode, 
    RepeatNode, 
    CaseNode, 
    AssignNode, 
    CompoundNode, 
    ProgramNode 
};

class ASTNode {
protected:
    ASTType NodeType;
};

class ExprNode : public ASTNode {};

class LiteralNode : public ExprNode {
public:
    enum class Kind { Int, Real, Bool, Char, String };
    Kind kind;
    std::string value;
};

class TypeNode : public ASTNode {
public:
    enum class Kind { Simple, Array, Record, Range, Enumerated };
    Kind kind;
    std::string name;                          // Simple
    std::shared_ptr<TypeNode> element_type;    // Array
    std::shared_ptr<ExprNode> low, high;       // Array bounds / Range
    std::vector<std::string> enum_values;      // Enumerated
    std::vector<std::pair<std::vector<std::string>, std::shared_ptr<TypeNode>>> fields;    // Record
};

class StmtNode : public ASTNode {};

class DeclNode : public ASTNode {
public:
    enum class Kind { Const, Type, Var, Proc, Func, Param };
    Kind kind;
    std::vector<std::string> names;
    std::shared_ptr<TypeNode> type_spec;       // Var, Param, Func return
    std::shared_ptr<ExprNode> value;           // Const
    std::vector<std::shared_ptr<DeclNode>> params;      // Proc/Func
    std::vector<std::shared_ptr<DeclNode>> local_decls; // Proc/Func
    std::shared_ptr<StmtNode> body;            // Proc/Func
    bool is_var_param = false;                 // Param
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
    std::shared_ptr<ExprNode> target;
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
    std::shared_ptr<StmtNode> body;
    std::shared_ptr<ExprNode> untilcondition;
};

class CaseNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> key;
    std::vector<std::pair<
        std::shared_ptr<ExprNode>,  
        std::shared_ptr<StmtNode>
    >> cases;
};

class AssignNode : public StmtNode {
public:
    std::shared_ptr<ExprNode> target;
    std::shared_ptr<ExprNode> value;
};

class CompoundNode : public StmtNode {
    std::vector<std::shared_ptr<StmtNode>> statements;
};

class ProgramNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<DeclNode>> declarations;
    std::shared_ptr<CompoundNode> main;
};