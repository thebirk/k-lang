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

Node* make_declassign(Token var, char *type, Node *expr)
{
    Node *n = make_node(NODE_VARDECLASSIGN);
    
    if(type == 0) {
        n->vardeclassign.type = 0;
    } else {
        n->vardeclassign.type = strdup(type);
    }
    n->vardeclassign.var = strdup(var.value);
    n->vardeclassign.expr = expr;
    
    return n;
}

Node* make_assignment(Token var, Node *expr)
{
    Node *n = make_node(NODE_VARASSIGN);
    
    n->varassign.var = strdup(var.value);
    n->varassign.expr = expr;
    
    return n;
}

Node* make_while(Node *cond, Node *block)
{
    Node *n = make_node(NODE_WHILE);
    
    n->nwhile.condition = cond;
    n->nwhile.block = block;
    
    return n;
}

Node* make_if(Node *cond, Node *block, Node *else_block)
{
    Node *n = make_node(NODE_IF);
    
    n->nif.condition = cond;
    n->nif.block = block;
    n->nif.else_block = else_block;
    
    return n;
}

// Remember to init the array to 0 and size to 0
Node* make_block()
{
    Node *n = make_node(NODE_BLOCK);
    
    n->block.stmts = 0;
    n->block.count = 0;
    
    return n;
}

void block_add_statement(Node *b, Node *stmt)
{
    b->block.count++;
    b->block.stmts = (Node**) realloc(b->block.stmts, sizeof(Node*)*b->block.count);
    b->block.stmts[b->block.count-1] = stmt;
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
