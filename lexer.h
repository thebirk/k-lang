#ifndef K_LEXER_H
#define K_LEXER_H

ENUM(int, TokenType)
{
    // Constants
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_IDENT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    
    // Syntax "operators"
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_LEFTPAR,
    TOKEN_RIGHTPAR,
    TOKEN_LEFTBRACE,
    TOKEN_RIGHTBRACE,
    TOKEN_LEFTBRACKET,
    TOKEN_RIGHTBRACKET,
    TOKEN_ASTERISK,
    TOKEN_DOT,
    TOKEN_EQUAL,
    
    // Keywords
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FUNC,
    TOKEN_WHILE,
    TOKEN_SWITCH,
    TOKEN_STRUCT,
    TOKEN_RETURN,
    
    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    /* ASTERISK is located earlier */
    TOKEN_SLASH,
    TOKEN_EQUALS,
    TOKEN_NEQUALS,
    
    TOKEN_EOF,
};

STRUCT(Token)
{
    TokenType type;
    char *filename; // Points to the filename owned by LexResult
    int line;
    int x;
    char *value;
};

STRUCT(LexResult)
{
    Token *tokens;
    int size;
    int mem_size;
    int increment_size;
    char *filename;
};

LexResult lex_file(const char *path);
LexResult lex_file_data(char *data, const char *filename);

const char* get_token_name(TokenType type);

#endif /* K_LEXER_H */
