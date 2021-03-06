#include "parser.h"

static Token __current;
static LexResult __tokens;
static int __offset;

void next()
{
    __offset++;
    __current = __tokens.tokens[__offset];
}

int istype(TokenType type)
{
    return __current.type == type;
}

int accept(TokenType type)
{
    if(__current.type == type) {
        next();
        return 1;
    } else {
    return 0;
    }
}

int expect(TokenType type, char *msg)
{
    if(accept(type)) {
        return 1;
    }
    printf("%s:%d:%d: %s\n", __current.filename, __current.line, __current.x, msg);
    exit(1);
    return 0;
}

void error(char *msg)
{
    printf("%s:%d:%d: %s\n", __current.filename, __current.line, __current.x, msg);
    exit(2);
}

Node* expression();

Node* factor()
{
     Token current = __current;
    if(accept(TOKEN_IDENT)) {
        if(accept(TOKEN_LEFTPAR)) {
            Node *f = make_node(NODE_FUNCCALL);
            f->funccall.func_name = make_identnode(current);
            f->funccall.argument_count = 0;
            f->funccall.arguments = 0;
            
            if(accept(TOKEN_RIGHTPAR)) {
                return f;
            }
            
            do {
                Node *expr = expression();
                f->funccall.argument_count++;
                f->funccall.arguments = realloc(f->funccall.arguments, sizeof(Node*) * f->funccall.argument_count);
                f->funccall.arguments[f->funccall.argument_count-1] = expr;
            } while(accept(TOKEN_COMMA));
            expect(TOKEN_RIGHTPAR, "Expected ')' after paramaters in function call!");
            
            return f;
        } else if(accept(TOKEN_LEFTBRACKET)) {
            
            Node *n = make_node(NODE_ARRAY_INDEX);
            n->array_index.expr_count = 0;
            n->array_index.expressions = 0;
            
            n->array_index.ident = make_identnode(current);
            
            do {
                Node *expr = expression();
                expect(TOKEN_RIGHTBRACKET, "Expected ']' while parsing array indexing!");
                
                n->array_index.expr_count++;
                n->array_index.expressions = realloc(n->array_index.expressions, sizeof(Node*) * n->array_index.expr_count);
                n->array_index.expressions[n->array_index.expr_count-1] = expr;
            } while(accept(TOKEN_LEFTBRACKET));
            
            return n;
        } else {
        return make_identnode(current);
        }
    } else if(accept(TOKEN_INTEGER)) {
        return make_integernode(current);
    } else if(accept(TOKEN_FLOAT)) {
        return make_floatnode(current);
    } else if(accept(TOKEN_LEFTPAR)) {
        Node *expr = expression();
        expect(TOKEN_RIGHTPAR, "Expected ')' after '(' while parsing expression!");
        return expr;
    } else {
        //printf("type: %d\n", __current.type);
        error("Expected ident, integer, float or '('");
    }
}

Node* term()
{
    Node *lhs = factor();
    while(__current.type == TOKEN_SLASH || __current.type == TOKEN_ASTERISK) {
        TokenType op = __current.type;
        next();
        Node *rhs = factor();
        
        if(op == TOKEN_ASTERISK) {
            lhs = make_binopnode(BINOP_MUL, lhs, rhs);
        } else {
            lhs = make_binopnode(BINOP_DIV, lhs, rhs);
        }
    }
    return lhs;
}

Node* expression()
{
    int prefix = 0;
    TokenType prefix_type;
    if(__current.type == TOKEN_PLUS || __current.type == TOKEN_MINUS) {
        prefix = 1;
        prefix_type = __current.type;
        next();
    }
    
    Node *lhs = term();
    // if prefix and prefix_type and negate if minus
    
    while(__current.type == TOKEN_PLUS || __current.type == TOKEN_MINUS) {
        TokenType op = __current.type;
        next();
        Node *rhs = term();
        
        if(op == TOKEN_PLUS) {
            lhs = make_binopnode(BINOP_ADD, lhs, rhs);
        }  else {
            lhs = make_binopnode(BINOP_SUB, lhs, rhs);
        }
    }
    
    return lhs;
}

Node* type()
{
    Node *nn = make_node(NODE_TYPE);
    TypeNode *n = &nn->ntype;
    n->pointer_count = 0;
    n->array_count = 0;
    
    if(accept(TOKEN_ASTERISK)) {
        n->pointer = 1;
        n->pointer_count++;
        
        while(accept(TOKEN_ASTERISK)) {
            n->pointer_count++;
        }
    }
    
    Token name = __current;
    expect(TOKEN_IDENT, "Expected type name!");
    
    n->ident = make_identnode(name);
    
    if(accept(TOKEN_LEFTBRACKET)) {
        n->array = 1;
        do {
        Node *expr = expression();
        expect(TOKEN_RIGHTBRACKET, "Expected ']' after expression!");
            n->array_count++;
            n->array_expr = realloc(n->array_expr, sizeof(Node*) * n->array_count);
            n->array_expr[n->array_count-1] = expr;
        } while(accept(TOKEN_LEFTBRACKET));
    }
    
    return nn;
}

Node* statement();
Node* block()
{
    Node *b = make_block();
    
    if(accept(TOKEN_LEFTBRACE)) {
        if(accept(TOKEN_RIGHTBRACE)) {
            return b;
        }
        do {
            Node *stmt = statement();
            block_add_statement(b, stmt);
        } while(__current.type != TOKEN_RIGHTBRACE);
        expect(TOKEN_RIGHTBRACE, "Expected '}' at the end of a block!");
        } else {
        Node *stmt = statement();
        block_add_statement(b, stmt);
    }
    
    return b;
}

Node* statement_semicolon()
{
    Token current = __current;
    
    if(accept(TOKEN_IDENT)) {
        if(accept(TOKEN_COLON)) {
            if(accept(TOKEN_EQUAL)) {
                // a := expr;
                Node *expr = expression();
                expect(TOKEN_SEMICOLON, "Expected semicolon!");
                 return make_declassign(current, 0, expr);
            } else {
                
                Node *t = type();
                
                if(accept(TOKEN_EQUAL)) {
                    Node *expr = expression();
                    expect(TOKEN_SEMICOLON, "Expected semicolon after variable decleration!");
                    
                    return make_declassign(current, t, expr);
                } else {
                    expect(TOKEN_SEMICOLON, "Expected semicolon after variable decleration!");
                    return make_decl(current, t);
                }
                
            }
        } else if(accept(TOKEN_EQUAL)) {
            Node *expr = expression();
            expect(TOKEN_SEMICOLON, "Expected semicolon!");
            return make_assignment(current, expr);
        } else if(accept(TOKEN_LEFTPAR)) {
            Node *f = make_node(NODE_FUNCCALL);
            f->funccall.func_name = make_identnode(current);
            f->funccall.argument_count = 0;
            f->funccall.arguments = 0;
            
            if(accept(TOKEN_RIGHTPAR)) {
                expect(TOKEN_SEMICOLON, "Expected ';' after function call!");
                return f;
            }
            do {
                Node *expr = expression();
                f->funccall.argument_count++;
                f->funccall.arguments = realloc(f->funccall.arguments, sizeof(Node*) * f->funccall.argument_count);
                f->funccall.arguments[f->funccall.argument_count-1] = expr;
            } while(accept(TOKEN_COMMA));
            expect(TOKEN_RIGHTPAR, "Expected ')' after paramaters in function call!");
            expect(TOKEN_SEMICOLON, "Expected ';' after function call!");
            return f;
        } else {
            error("Expected ':', '=' or '(' after ident!");
        }
    } else if(accept(TOKEN_RETURN)) {
        if(accept(TOKEN_SEMICOLON)) {
            return make_return(0);
        } else {
            Node *expr = expression();
            expect(TOKEN_SEMICOLON, "Expected ';' after return expression!");
            return make_return(expr);
        }
    }
    
    error("Nope");
}

Node* statement()
{
    Token current = __current;
    if(accept(TOKEN_WHILE)) {
        Node *expr = expression();
        Node *b = block();
        return make_while(expr, b);
    } else if(accept(TOKEN_IF)) {
        Node *expr = expression();
        Node *b = block();
        Node *eb = 0;
        if(accept(TOKEN_ELSE)) {
            eb = block();
        }
        
        return make_if(expr, b, eb);
    } else {
        return statement_semicolon();
    }
}

Node* program()
{
    Node *p = make_program();
     do {
    if(accept(TOKEN_FUNC)) {
        Token name = __current;
        expect(TOKEN_IDENT, "Expected function name after 'func'!");
        expect(TOKEN_LEFTPAR, "Expected '(' after function name!");
        
        int argument_count = 0;
         Node **argument_idents = 0;
         Node **argument_types = 0;
        
        if(!accept(TOKEN_RIGHTPAR)) {
            do {
            Token var = __current;
            expect(TOKEN_IDENT, "Expected argument name!");
                expect(TOKEN_COLON, "Expected colon after argument name!");
                Node *t = type();
                
                argument_count++;
                argument_idents = realloc(argument_idents, sizeof(Node*)*argument_count);
                argument_types = realloc(argument_types, sizeof(Node*)*argument_count);
                argument_idents[argument_count-1] = make_identnode(var);
                argument_types[argument_count-1] = t;
                
        } while(accept(TOKEN_COMMA));
        
        expect(TOKEN_RIGHTPAR, "Expected ')' after declaring function arguments!");
    }
    
    Node *function_type = 0;
    if(accept(TOKEN_COLON)) {
        function_type = type();
    }
    
    if(accept(TOKEN_SEMICOLON)) {
        program_add_func(p, make_funcdecl(make_identnode(name), function_type, argument_count, argument_idents, argument_types));
    } else {
        Node *b = block();
        program_add_func(p, make_funcdef(make_identnode(name), function_type, b, argument_count, argument_idents, argument_types));
    }
    
}
    
} while(__current.type != TOKEN_EOF);
    return p;
}

Node* parse(LexResult result)
{
    __tokens = result;
    __offset = 0;
    __current = __tokens.tokens[__offset];
    
    Node* expr = program();
    return expr;
}