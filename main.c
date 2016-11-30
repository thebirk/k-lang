/*

Copyright (c) 2016 Aleksander B. Birkeland

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/
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
#ifdef HAS_JAVA_TREEVIEW
#include "treeview/treeview.c"
#endif /* HAS_JAVA_TREEVIEW */
#include "typesystem.c"
#include "llvm-gen/llvm-gen.c"

void print_lex_result(LexResult result)
{
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
}

int main(int argc, char **argv)
{
    LexResult result = lex_file("current.k.c");
    
    Node *program = parse(result);
    
    #ifdef HAS_JAVA_TREEVIEW
    //view_ast(program);
    #else
    print_node(program);
    #endif /* HAS_JAVA_TREEVIEW */
    
    TypeSystem ts = {0};
    typesys_init(&ts);
    add_default_types(&ts);
    
    llvm_gen(program, "out.bc");
    
    return 0;
}
