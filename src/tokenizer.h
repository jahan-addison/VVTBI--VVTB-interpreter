/***************************************
   tokenizer.h, @format.new-line  lf
                @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
****************************************/
#ifndef _TOKENIZER_H__
#define _TOKENIZER_H__

enum {
  T_ERROR = 1,

  T_EOF,
  T_NUMBER,
  T_LETTER,
  T_STRING,
  T_EQUAL,
  T_LT,
  T_GT,
  T_LT_EQ,
  T_GT_EQ,
  T_NOT_EQUAL,
  T_SEPERATOR,
  T_MINUS,
  T_PLUS,
  T_ASTERISK,
  T_SLASH,
  T_LET,
  T_IF,
  T_THEN,
  T_PRINT,
  T_REM,
  T_GOTO,
  T_LEFT_PAREN,
  T_RIGHT_PAREN,
  T_EOL
};

void  tokenizer_init         (const char *source);
int   tokenizer_finished     (void);
int   tokenizer_variable_num (void);
char *tokenizer_string       (void);
int   tokenizer_num          (void);
void  reset                  (int to);
int   tokenizer_token        (void);
void  tokenizer_next         (void);

#endif /* _TOKENIZER_H__ */