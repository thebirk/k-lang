#include "semantics.h"

void check_globals(Node *program, TypeSystem *ts)
{
    
}

void check_functions(Node *np, TypeSystem *ts)
{
    ProgramNode *program = &np->program;
    // Check return types
    for(int i = 0; i < program->func_count; i++) {
        Node *n = program->functions[i];
        
        if(n->type == NODE_FUNCDEF) {
            
            FuncDefNode *f = &n->funcdef;
            
        } else if(n->type == NODE_FUNCDECL) {
            
            
            
        } else {
            printf("Aww shit you fucked up in semantics.c:check_funtions()\n");
            exit(-1);
        }
    }
}

void type_check(Node *program, TypeSystem *ts)
{
    check_globals(program, ts);
    check_functions(program, ts);
}