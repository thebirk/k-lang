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
    
    return n;
}

static void print_indents(int n)
{
    for(int i = 0; i < n; i++) {
        printf("  ");
    }
}

 void print_node(Node *n)
{
    static int ident = 0;
    print_indents(ident);
    switch(n->type) {
        case NODE_BINOP: {
            BinOpNode op = n->binop;
            //print_indents(ident);
            printf("BinOpType: %d\n", op.type);
            print_indents(ident);
            printf("LHS:\n");
            ident++;
            print_node(op.lhs);
            ident--;
            print_indents(ident);
            printf("RHS:\n");
            ident++;
            print_node(op.rhs);
            ident--;
        } break;
        
        case NODE_CONSTANT: {
            ConstantNode con = n->constant;
            //print_indents(ident);
            printf("ConstantType: %d\n", con.type);
            print_indents(ident);
            printf("Value: %s\n", con.value);
        } break;
        
        case NODE_IDENT: {
            IdentNode id = n->ident;
            //print_indents(ident);
            printf("Ident: %s\n", id.value);
        } break;
        
        default: {
            printf("Unimplemented node in print_node()!\n");
        } break;
    }
}
