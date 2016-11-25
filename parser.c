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
            // Parse func call
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

Node* statement();
Node* block()
{
    // Either a single statement or
    // multiple statement within braces
    
    Node *b = make_block();
    
    if(accept(TOKEN_LEFTBRACE)) {
        do {
            Node *stmt = statement();
            block_add_statement(b, stmt);
        } while(accept(TOKEN_SEMICOLON));
        expect(TOKEN_SEMICOLON, "Expected ';' after the last statement in a block!");
        expect(TOKEN_RIGHTBRACE, "Expected '}' at the end of a block!");
        
        
    } else {
        Node *stmt = statement();
        expect(TOKEN_SEMICOLON, "Expected ';' after statement!");
        block_add_statement(b, stmt);
    }
    
    return b;
}

Node* statement()
{
    Token current = __current;
    
    if(accept(TOKEN_IDENT)) {
        if(accept(TOKEN_COLON)) {
            if(accept(TOKEN_EQUAL)) {
                // a := expr;
                Node *expr = expression();
                
                 return make_declassign(current, 0, expr);
            } else {
                // TODO: Implement a type specifier resolver thingy
                
            }
        } else if(accept(TOKEN_EQUAL)) {
            Node *expr = expression();
            return make_assignment(current, expr);
        } else if(accept(TOKEN_LEFTPAR)) {
            // TODO: Parse arguments
            // do while style is probably the best
        } else {
            error("Expected ':', '=' or '(' after ident!");
        }
    } else if(accept(TOKEN_WHILE)) {
        Node *expr = expression();
        Node *b = block();
        return make_while(expr, b);
    }
}

Node* program()
{
    return 0;
}

Node* parse(LexResult result)
{
    __tokens = result;
    __offset = 0;
    __current = __tokens.tokens[__offset];
    
    Node* expr = expression();
}