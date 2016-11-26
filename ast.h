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
    NODE_TYPE,
    NODE_RETURN,
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
     Node *type;
      char *var;
};

STRUCT(VarAssignNode)
{
      char*var;
    Node *expr;
};

STRUCT(VarDeclAssignNode)
{
     Node *type;
      char *var;
    Node *expr;
};

STRUCT(FuncCallNode)
{
    char *func_name;
    int argument_count;
    Node **arguments;
};

STRUCT(FuncDefNode)
{
     Node *name;
     Node *type;
    Node *block;
    int argument_count;
     Node **argument_idents;
    Node **argument_types;
};

STRUCT(FuncDeclNode)
{
     Node *name;
     Node *type;
    int argument_count;
     Node **argument_idents;
    Node **argument_types;
};

STRUCT(ProgramNode)
{
    int func_count;
    Node **functions;
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

STRUCT(TypeNode)
{
    int pointer;
    int pointer_count;
    Node *ident;
    int array;
    int array_count;
    Node **array_expr;
};

STRUCT(ReturnNode)
{
    Node *expr;
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
        FuncDefNode funcdef;
        ProgramNode program;
        BlockNode block;
        WhileNode nwhile;
        IfNode nif;
        TypeNode ntype;
        ReturnNode nreturn;
    };
};

Node* make_node(NodeType type);
Node* make_identnode(Token t);
Node* make_decl(Token var, Node *type);
Node* make_declassign(Token var, Node *type, Node *expr);
Node* make_assignment(Token var, Node *expr);
Node* make_while(Node *cond, Node *block);
Node* make_if(Node *cond, Node *block, Node *else_block);
Node* make_return(Node *expr);

Node* make_funcdecl(Node *name, Node *type, int argument_count, Node **argument_idents, Node **argument_types);
Node* make_funcdef(Node *name, Node *type, Node *block, int argument_count, Node **argument_idents, Node **argument_types);

// Remember to init the array to 0 and size to 0
Node* make_block();
void block_add_statement(Node *block, Node *stmt);

Node* make_program();
void program_add_func(Node *program, Node *func);

void print_node(Node *n);

#endif /* K_AST_H */
