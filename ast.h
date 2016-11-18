#ifndef K_AST_H
#define K_AST_H

ENUM(int, NodeType)
{
    NODE_BINOP = 0,
    NODE_CONSTANT,
    NODE_IDENT,
    NODE_FUNCDEF,
    NODE_FUNCDECL,
    NODE_FUNCCALL,
    NODE_VARDECL,
    NODE_VARASSIGN,
    NODE_PROGRAM,
};

ENUM(int, BinOpType)
{
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
    BINOP_MOD,
};

ENUM(int, ConstantType)
{
    CONST_INTEGER = 0,
    CONST_FLOAT,
    CONST_STRING,
};

STRUCT(BinOpNode)
{
    BinOpType type;
    Node *lhs;
    Node *rhs;
};

STRUCT(ConstantNode)
{
    ConstantType type;
    char *value;
};

STRUCT(IdentNode)
{
    char *value;
};

STRUCT(VarDeclNode)
{
    char *type;
    char *var;
};

STRUCT(VarAssignNode)
{
    char *var;
    Node *expr;
};

STRUCT(FuncCallNode)
{
    char *func_name;
    // TODO: Store argument count and types
};

STRUCT(FuncDeclNode)
{
    char *name;
    char *type; // TODO: Does 0 == void or does "void" == void
    // TODO: Store argument count, type, and names
    Node *block; // Use blocks or arrays of stmts?
};

STRUCT(FuncDefNode)
{
    char *name;
    char *type;
    // TODO: Store argument count, type and names
};

STRUCT(ProgramNode)
{
    // Root of tree
};

STRUCT(Node)
{
    NodeType type;
    
    union {
        BinOp binop;
        ConstantNode constant;
        IdentNode ident;
        VarDeclNode vardecl;
        VarAssignNode varassign;
        FuncCallNode funccall;
        FuncDeclNode funcdecl;
        FuncDefNode funcdec;
        ProgramNode program;
    };
};

Node* make_node(NodeType type);
Node* make_identnode(Token t);

#endif /* K_AST_H */