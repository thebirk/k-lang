#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#define _strdup strdup
#endif /* _WIN32 */

#define STRUCT(name) typedef struct name name; struct name
#define ENUM(type, name) typedef type name; enum name##_

#include "lexer.c"
#include "ast.c"
#include "parser.c"

int main(int argc, char **argv)
{
    LexResult result = lex_file("current.k");
    
    for(int i = 0; i < result.size; i++) {
        Token t = result.tokens[i];
        
        printf("%s:%d:%d: %s", t.filename, t.line, t.x, get_token_name(t.type));
        
        if(t.type == TOKEN_STRING) {
            printf(": %s\n", t.value);
        } else if(t.type == TOKEN_IDENT) {
            printf(": %s\n", t.value);
        } else if(t.type == TOKEN_INTEGER) {
            printf(": %s\n", t.value);
         } else {
            printf("\n");
        }
    }
    
    Node *program = parse(result);
    print_node(program);
    
    return 0;
}

