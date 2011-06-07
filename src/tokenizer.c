/***************************************
   tokenizer.c, @format.new-line  lf
                @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
****************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "io.h"
#include "tokenizer.h"

#define MAX_STRING_LEN 50
#define MAX_NUMBER_LEN 6

/*****************/
/* current token */
/*****************/
static int token;

/***********************/
/* scanner's "pointer" */
/***********************/
union Pointer {
  char string[MAX_STRING_LEN+1];
  int number;
  int  letter;
} text;

struct keyword_token {
  char *keyword;
  int   token;
};
/*********************/
/* language keywords */
/*********************/
static const struct keyword_token keywords[] = {
  {"let",   T_LET},
  {"if",    T_IF},
  {"then",  T_THEN},
  {"print", T_PRINT},
  {"rem",   T_REM},
  {"goto",  T_GOTO},
  {NULL,    T_ERROR}
};

static int get_next_token (void);

/******************************************************************************/

/* initialize our file stream and tokenizer */

void tokenizer_init (FILE *source)
{
  io_init(source);
  token = get_next_token();
}

/* a relational or comparison token */

static int token_relation (void)
{
  int next;
  /* peek at the next token */
  next = io_peek();
  switch (io_current())
  {
    case '=':
      io_next();
      return T_EQUAL;
      break;
    case '<':
      io_next();
      if (next == '>')
      {
        io_next();
        return T_NOT_EQUAL;
      }
      else if (next == '=')
      {
        io_next();
        return T_LT_EQ;
      }
      else
      {
        io_next();
        return T_LT;
      }
      break;
    case '>':
      io_next();
      if (next == '=')
      {
        io_next();
        return T_GT_EQ;
      }
      else
      {
        io_next();
        return T_GT;
      }
      break;
  }
  return 0;
}

/* an operation (arithmetic) token */

static int token_operation (void)
{
  switch (io_current())
  {
    case '(': return T_LEFT_PAREN;
      break;
    case ')': return T_RIGHT_PAREN;
      break;
    case '+': return T_PLUS;
      break;
    case '-': return T_MINUS;
      break;
    case '/': return T_SLASH;
      break;
    case '*': return T_ASTERISK;
      break;
  }
  return 0;
}

/* a string token, set our pointer string value */

static int token_string (void)
{
  size_t i;
  i = 0;
  io_next(); /* skip current `"' character */
  do
  {
    if (i >= MAX_STRING_LEN) break;
  text.string[i++] = (unsigned char)io_current();
    io_next();
  } while(io_current() != '"');
    io_next();
  if (i > MAX_STRING_LEN)
    text.string[MAX_STRING_LEN] = 0;
  else
    text.string[i] = 0;
  return T_STRING;
}

/* a keyword token */

static int token_keyword (void)
{
  size_t i;
  struct keyword_token const *kt;
  for (i = 0, kt = keywords; kt->keyword; kt++)
  {
    while (toupper(kt->keyword[i]) == io_current())
    {
      i++;
      if (!(kt->keyword[i]))
      {
        /* the REM (comment) statement */
        if (kt->token == T_REM)
        {
          /* skip all characters until newline */
          while(io_current() != '\r'
          && io_current() != '\n')
            io_next();
          return T_REM;
        }
          io_next();
          return kt->token;
      }
      io_next();
    }
  }
  return 0;
}

/* a number (whole number) token, set our Pointer */

static int token_number (void)
{
  int i;
  char number[MAX_NUMBER_LEN+1];
  for (i = 0; MAX_NUMBER_LEN; i++)
  {
    if (!isdigit(io_current()))
    {
      if (i > 0)
      {
        number[i]   = 0;
        text.number = strtol(number, NULL, 10);
        return T_NUMBER;
      }
    }
    number[i] = io_current();
    io_next();
  }
  return T_ERROR;
}

/* get the next token from the scanner */

static int get_next_token (void)
{
  int c, is_token;
  c = io_current();
  /* EOF token */
  if (c == EOF) return T_EOF;
  /* seperator token */
  if (c == ';' || c == ',')
  {
    io_next(); return T_SEPERATOR;
  }
  /* new-line token */
  if (c == '\r')
  {
    c = io_peek();
    io_next();
    if (c == '\n')
    {
      io_next(); return T_NEWLINE;
    }
    return T_NEWLINE;
  }
  else if (c == '\n')
  {
    io_next(); return T_NEWLINE;
  }
  /* relation token? */
  is_token = token_relation();
  if (is_token) return is_token;
  /* operation token? */
  is_token = token_operation();
  if (is_token)
  {
    io_next(); return is_token;
  }
  /* keyword token? */
  is_token = token_keyword();
  if (is_token) return is_token;
  /* string token */
  if (c == '"') return token_string();
  /* number token */
  if (isdigit(c)) return token_number();
  /* letter token? */
  if (c >= 'a' && c <= 'z')
  {
    text.letter = c;
    io_next();
    return T_LETTER;
  }
  io_next();
  /* error! */
  return T_ERROR;
}

/* move to the next token in the scanner */

void tokenizer_next (void)
{
  if (tokenizer_finished())
    return;
  /* skip whitespace */
  while (io_current() == ' ' ||
  io_current() == '\t')
    io_next();
  token = get_next_token();
}

/* current token in scanner */

int tokenizer_token (void)
{
  return token;
}

/* string from scanner Pointer */

char *tokenizer_string(void)
{
  return text.string;
}

/* number from scanner Pointer */

int tokenizer_num (void)
{
  return text.number;
}

/* variable number from scanner Pointer */

int tokenizer_variable_num (void)
{
  return text.letter - 'a';
}

/* reset token */

void reset (void)
{
  token = T_NUMBER;
}

/* scanner is complete */

int tokenizer_finished (void)
{
  if (token == T_EOF)
    /* close I/O */
    io_close();
  return token == T_EOF;
}