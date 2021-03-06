#include "lexer.h"

static void result_add_token(LexResult *r, Token t)
{
    r->size++;
    if(r->size >= r->mem_size) {
        r->mem_size += r->increment_size;
        r->tokens = (Token*) realloc(r->tokens, r->mem_size*sizeof(t));
    }
    
    r->tokens[r->size-1] = t;
}

static inline Token make_token_(TokenType type, int line, int x, char *filename)
{
    return (Token){type, filename, line, x, 0};
}

static inline int is_letter(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c  <= 'Z');
}

static inline int is_digit(int c)
{
    return (c >= '0' && c <= '9');
}

static inline int is_alphanum(int c)
{
    return is_letter(c) || is_digit(c);
    }

    static inline int is_ident_special(int c)
    {
        return (c == '$') || (c == '_');
    }
    
static inline int is_ident_start(int c)
{
    return is_letter(c) || is_ident_special(c);
}

static inline int is_ident(int c)
{
    return is_alphanum(c) || is_ident_special(c);
}

static inline int is_num(int c)
{
    return is_digit(c) || (c == '.');
}

LexResult lex_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    
    char *data = (char*) malloc(sizeof(char)*(len+1));
    fread(data, sizeof(char)*len, 1, f);
    data[len] = 0;
    
    fclose(f);
    
    LexResult result = lex_file_data(data, path);
    
    free(data);
    return result;
}

LexResult lex_file_data(char *data, const char *filename)
{
    LexResult result = {0};
    result.tokens = 0;
    result.size = 0;
    result.mem_size = 0;
    result.increment_size = 256;
    result.filename = strdup(filename);
    
    int line = 1;
    int x = 1;
    
#define make_token(type) make_token_(type, line, x, result.filename)
    char *ptr = data;
    while(*ptr) {
        
        if(*ptr == '/' && *(ptr+1) == '/') {
            ptr += 2;
            while(*ptr && (*ptr != '\n')) {
                ptr++;
            }
            if(*ptr == 0) continue;
            ptr++;
            if(*ptr == 0) continue;
            x = 1;
            line++;
            continue;
        }
        
        if(*ptr == '\r') {
            ptr++;
            continue;
        }
        
        if(*ptr == ' ' || *ptr == '\t') {
            x++;
            ptr++;
            continue;
        }
        
        if(*ptr == '\n') {
            x = 1;
            line++;
            ptr++;
            continue;
        }
        
        // Double tokens
        if(*ptr == '=' && *(ptr+1) == '=') {
            result_add_token(&result, make_token(TOKEN_EQUALS));
            ptr += 2;
            x += 2;
            continue;
        }
#define DOUBLE_TOKEN(t1, t2, type)\
        else if(*ptr == t1 && *(ptr+1) == t2) {\
            result_add_token(&result, make_token(type));\
            ptr += 2;\
            x += 2;\
            continue;\
        }
        
        DOUBLE_TOKEN('!', '=', TOKEN_NEQUALS)
        
#undef DOUBLE_TOKEN
        
        // Single tokens
        switch(*ptr) {
#define SINGLE_TOKEN(tok, type) case tok: {result_add_token(&result, make_token(type)); x++; ptr++; continue; } break;
            SINGLE_TOKEN('+', TOKEN_PLUS);
            SINGLE_TOKEN('-', TOKEN_MINUS);
            SINGLE_TOKEN('*', TOKEN_ASTERISK);
            SINGLE_TOKEN('/', TOKEN_SLASH);
            SINGLE_TOKEN(':', TOKEN_COLON);
            SINGLE_TOKEN(';', TOKEN_SEMICOLON);
            SINGLE_TOKEN('.', TOKEN_DOT);
            SINGLE_TOKEN(',', TOKEN_COMMA);
            SINGLE_TOKEN('=', TOKEN_EQUAL);
            SINGLE_TOKEN('(', TOKEN_LEFTPAR);
            SINGLE_TOKEN(')', TOKEN_RIGHTPAR);
            SINGLE_TOKEN('[', TOKEN_LEFTBRACKET);
            SINGLE_TOKEN(']', TOKEN_RIGHTBRACKET);
            SINGLE_TOKEN('{', TOKEN_LEFTBRACE);
            SINGLE_TOKEN('}', TOKEN_RIGHTBRACE);
            #undef SINGLE_TOKEN
        }
        
        if(*ptr == '"') {
            char *start = ptr;
            ptr++;
            while(*ptr && *ptr != '"') {
                ptr++;
            }
            if(*ptr == 0) {
                printf("%s:%d:%d: Unexpected end of file, while parsing string!\n", result.filename, line, x);
                exit(-1);
            }
            ptr++;
            
            int len = ptr - start;
            char *str = (char*) malloc(sizeof(char)*(len+1));
            memcpy(str, start, len);
            str[len] = 0;
            
            Token t = make_token(TOKEN_STRING);
            t.value = str;
            x += len;
            result_add_token(&result, t);
            continue;
        }
        
        if(is_ident_start(*ptr)) {
            char *start = ptr;
            while(*ptr && is_ident(*ptr)) {
                ptr++;
            }
            
            int len = ptr - start;
            char *str = (char*) malloc(sizeof(char)*(len+1));
            memcpy(str, start, len);
            str[len] = 0;
            
            Token t = make_token(TOKEN_IDENT);
            t.value = str;
            x += len;
            result_add_token(&result, t);
            continue;
        }
        
        if(is_num(*ptr)) {
            int found_dot = 0;
            if(*ptr == '.') found_dot = 1;
            
            // TODO: Handle '0x' hexadecimals
            
            char *start = ptr;
            while(*ptr && is_num(*ptr)) {
                if(*ptr == '.') {
                    if(found_dot) {
                        printf("%s:%d:%d: Found second '.' while parsing number!\n", result.filename, line, x);
                        exit(-1);
                    } else {
                        found_dot = 1;
                    }
                }
                ptr++;
            }
            
            int len = ptr - start;
            char *str = (char*) malloc(sizeof(char)*(len+1));
            memcpy(str, start, len);
            str[len] = 0;
            
            Token t = make_token(TOKEN_INTEGER);
            t.value = str;
            x += len;
            result_add_token(&result, t);
            continue;
        }
        
        printf("%s:%d:%d: Unknown token '%c' 0x%X\n", result.filename, line, x, *ptr, *ptr);
        ptr++;
        x++;
    }
    
    result_add_token(&result, make_token(TOKEN_EOF));
    
    // Handle keywords
    for(int i = 0; i < result.size; i++) {
        Token *t = &result.tokens[i];
        
        if(t->type == TOKEN_IDENT) {
            if(strcmp("func", t->value) == 0) {
                t->type = TOKEN_FUNC;
                free(t->value);
                continue;
            }
#define KEYWORD(str, _type) else if(strcmp(str, t->value) == 0) { t->type = _type; free(t->value); continue; }
            KEYWORD("true", TOKEN_TRUE)
                KEYWORD("false", TOKEN_FALSE)
                KEYWORD("if", TOKEN_IF)
                KEYWORD("else", TOKEN_ELSE)
                KEYWORD("while", TOKEN_WHILE)
                KEYWORD("struct", TOKEN_STRUCT)
                KEYWORD("return", TOKEN_RETURN)
            #undef KEYWORD
        }
    }
    
    return result;
}

const char* get_token_name(TokenType type)
{
    switch(type) {
#define TOKEN(tok) case tok: {return #tok; } break;
        TOKEN(TOKEN_INTEGER);
        TOKEN(TOKEN_FLOAT);
        TOKEN(TOKEN_STRING);
        TOKEN(TOKEN_CHAR);
        TOKEN(TOKEN_IDENT);
        TOKEN(TOKEN_TRUE);
        TOKEN(TOKEN_FALSE);
        TOKEN(TOKEN_COMMA);
        TOKEN(TOKEN_COLON);
        TOKEN(TOKEN_SEMICOLON);
        TOKEN(TOKEN_LEFTPAR);
        TOKEN(TOKEN_RIGHTPAR);
        TOKEN(TOKEN_LEFTBRACE);
        TOKEN(TOKEN_RIGHTBRACE);
        TOKEN(TOKEN_LEFTBRACKET);
        TOKEN(TOKEN_RIGHTBRACKET);
        TOKEN(TOKEN_ASTERISK);
        TOKEN(TOKEN_DOT);
        TOKEN(TOKEN_EQUAL);
        TOKEN(TOKEN_IF);
        TOKEN(TOKEN_FUNC);
        TOKEN(TOKEN_WHILE);
        TOKEN(TOKEN_SWITCH);
        TOKEN(TOKEN_STRUCT);
        TOKEN(TOKEN_PLUS);
        TOKEN(TOKEN_MINUS);
        TOKEN(TOKEN_SLASH);
        TOKEN(TOKEN_EQUALS);
        TOKEN(TOKEN_NEQUALS);
        TOKEN(TOKEN_EOF);
        TOKEN(TOKEN_RETURN);
        TOKEN(TOKEN_ELSE);
        #undef TOKEN
    }
    
    return "(no name)";
}
