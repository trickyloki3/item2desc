#ifndef i2d_script_h
#define i2d_script_h

#include "i2d_util.h"

enum i2d_token_type {
    I2D_HEAD,
    I2D_CURLY_OPEN,
    I2D_CURLY_CLOSE,
    I2D_PARENTHESIS_OPEN,
    I2D_PARENTHESIS_CLOSE,
    I2D_COMMA,
    I2D_SEMICOLON,
    I2D_IDENTIFIER
};

struct i2d_token {
    enum i2d_token_type type;
    i2d_buf * buffer;
    struct i2d_token * next;
    struct i2d_token * prev;
};

typedef struct i2d_token i2d_token;

struct i2d_lexer {
    i2d_token * list;
    size_t size;
    i2d_buf * tokens;
};

typedef struct i2d_lexer i2d_lexer;

int i2d_token_init(i2d_lexer *, i2d_token **, enum i2d_token_type);
int i2d_token_write(i2d_token *, void *, size_t);
void i2d_token_append(i2d_token *, i2d_token *);
void i2d_token_remove(i2d_token *);

int i2d_lexer_init(i2d_lexer **);
void i2d_lexer_deit(i2d_lexer **);
void i2d_lexer_reset(i2d_lexer *);
int i2d_lexer_tokenize(i2d_lexer *, i2d_str *);

#if i2d_debug
int i2d_lexer_test(void);
#endif
#endif
