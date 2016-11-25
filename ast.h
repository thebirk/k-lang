#ifndef K_AST_H
#define K_AST_H
#include "lexer.h"

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
    NODE_VARDECLASSIGN,
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_WHILE,
    NODE_IF,
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

typedef struct Node Node;

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

STRUCT(VarDeclAssignNode)
{
    char *type;
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
    char *type;
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
    char stub;
};

STRUCT(BlockNode)
{
    Node **stmts;
    int count;
};

STRUCT(WhileNode)
{
    Node *condition;
    Node *block;
};

STRUCT(IfNode)
{
    Node *condition;
    Node *block;
    Node *else_block;
};

STRUCT(Node)
{
    NodeType type;
    
    union {
        BinOpNode binop;
        ConstantNode constant;
        IdentNode ident;
        VarDeclNode vardecl;
        VarAssignNode varassign;
        VarDeclAssignNode vardeclassign;
        FuncCallNode funccall;
        FuncDeclNode funcdecl;
        FuncDefNode funcdec;
        ProgramNode program;
        BlockNode block;
        WhileNode nwhile;
        IfNode nif;
    };
};

Node* make_node(NodeType type);
Node* make_identnode(Token t);
Node* make_declassign(Token var, char *type, Node *expr);
Node* make_assignment(Token var, Node *expr);
Node* make_while(Node *cond, Node *block);
Node* make_if(Node *cond, Node *block, Node *else_block);

// Remember to init the array to 0 and size to 0
Node* make_block();
void block_add_statement(Node *block, Node *stmt);

void print_node(Node *n);

#endif /* K_AST_H */
