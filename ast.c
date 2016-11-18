#include "ast.h"

Node* make_node(NodeType type)
{
    Node *n = (Node*) malloc(sizeof(Node));
    n->type = type;
    return n;
}

Node* make_identnode(Token t)
{
    Node *n = make_node(NODE_IDENT);
    
    n->ident.value = strdup(t.value);
    
    return n;
}

Node* make_integernode(Token t)
{
    Node *n = make_node(NODE_CONSTANT);
    
    n->constant.type = CONST_INTEGER;
    n->constant.value = strdup(t.value);
    
    return n;
}

Node* make_floatnode(Token t)
{
    Node *n = make_node(NODE_CONSTANT);
    
    n->constant.type = CONST_FLOAT;
    n->constant.value = strdup(t.value);
    
    return n;
}

Node* make_binopnode(BinOpType type, Node *lhs, Node *rhs)
{
    Node *n = make_node(NODE_BINOP);
    
    n->binop.type = type;
    n->binop.lhs = lhs;
    n->binop.rhs = rhs;
}