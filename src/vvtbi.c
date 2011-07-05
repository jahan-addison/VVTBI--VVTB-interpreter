/***********************************
   vvtbi.c, @format.new-line  lf
            @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "config.h"
#include "io.h"
#include "tokenizer.h"
#include "vvtbi.h"

/* Token strings. */
static const char *token_strings[] =
{
  NULL,

  "T_ERROR",
  "T_EOF",
  "T_NUMBER",
  "T_LETTER",
  "T_STRING",
  "T_EQUAL",
  "T_LT",
  "T_GT",
  "T_LT_EQ",
  "T_GT_EQ",
  "T_NOT_EQUAL",
  "T_SEPERATOR",
  "T_MINUS",
  "T_PLUS",
  "T_ASTERISK",
  "T_SLASH",
  "T_LET",
  "T_IF",
  "T_THEN",
  "T_PRINT",
  "T_REM",
  "T_GOTO",
  "T_LEFT_PAREN",
  "T_RIGHT_PAREN",
  "T_EOL"
};

/* Debugging error constants. */
enum {
  E_ERROR = 1, E_WARNING
};

/* The variable container. (a - z) */
#define VVTBI_VARIABLES 26
static int variables[26];

static int expression (void);
static void line_statement (void);
static void statement (void);

/******************************************************************************/

/**
 * dprintf
 *
 * @param format Message format.
 * @param error Error code.
 * @param ... Additional arguments.
 * @return void
 */

static void dprintf(const char *format,
  int error, ...)
{
  va_list args;

  va_start(args, error);
  vfprintf(stderr, format, args);
  va_end(args);
  /* Exit on E_ERROR. */
  if (error == E_ERROR)
    exit(EXIT_FAILURE);
}

/**
 * set_variable
 *
 * @param place Position in container
 * @param value Value to place in container.
 * @return void
 */

static void set_variable (int place, int value)
{
  if (place >= 0 && place <= VVTBI_VARIABLES)
    variables[place] = value;
}

/**
 * get_variable
 *
 * @param place Poisition in container
 * @return value from container
 */

static int get_variable (int place)
{
  if (place >= 0 && place <= VVTBI_VARIABLES)
    return variables[place];
  return 0;
}

/**
 * vvtbi_init
 *
 * @param source Source code file.
 * @return void
 */

void vvtbi_init (const char *source)
{
  tokenizer_init(source);
  /* initialize the variable container. */
  memset(variables, 0, VVTBI_VARIABLES);
}

/**
 * vvtbi_token
 *
 * @param token Token.
 * @return Token string.
 */

const char *vvtbi_token (int token)
{
  /* Boundary check. */
  if (token < T_ERROR || token > T_EOL)
    return "T_ERROR";
  return token_strings[token];
}

/**
 * accept
 *
 * @param token Expected token.
 * @return void
 */

static void accept (int token)
{
  char string[10];
  if (token != tokenizer_token())
  {
    /* Token was unexpected. */
    to_string(string, sizeof string);
    dprintf("*vvtbi.c: unexpected `%s' "
      "near `%s', expected: `%s'\n",
      E_ERROR,
      vvtbi_token(tokenizer_token()),
      /* If empty, EOF! */
      ((strlen(string)) ? string : "EOF"),
      vvtbi_token(token));
  }
  tokenizer_next();
}

/**
 * facor
 *
 * @param void
 * @return r Factorized data.
 */

static int factor (void)
{
  int r;
  switch (tokenizer_token())
  {
    case T_NUMBER:
      r = tokenizer_num();
      accept(T_NUMBER);
      break;
    case T_LEFT_PAREN:
      accept(T_LEFT_PAREN);
      r = expression();
      accept(T_RIGHT_PAREN);
      break;
    default:
      r = get_variable(
        tokenizer_variable_num());
      accept(T_LETTER);
      break;
  }
  return r;
}

/**
 * term
 *
 * @param void
 * @return f1 The term.
 */

static int term (void)
{
  int f1, f2, op;
  f1 = factor();
  op = tokenizer_token();
  while (op == T_ASTERISK ||
  op == T_SLASH)
  {
    tokenizer_next();
    f2 = factor();
    switch (op)
    {
      case T_ASTERISK:
        f1 = f1 * f2;
        break;
      case T_SLASH:
        if (f2 == 0)
        {
          /* Divide by zero. */
          dprintf(
            "*warning: divide by zero\n",
            E_WARNING);
          f1 = 0;
        }
        else
        {
          f1 = f1 / f2;
        }
        break;
    }
    op = tokenizer_token();
  }
  return f1;
}

/**
 * expression
 *
 * @param void
 * @return t1 Evaluated expression value.
 */

static int expression (void)
{
  int t1, t2, op;
  t1 = term();
  op = tokenizer_token();
  while(op == T_PLUS ||
  op == T_MINUS)
  {
    tokenizer_next();
    t2 = term();
    switch (op)
    {
      case T_PLUS:
        t1 = t1 + t2;
        break;
      case T_MINUS:
        t1 = t1 - t2;
        break;
    }
    op = tokenizer_token();
  }
  return t1;
}

/**
 * relation
 *
 * @param void
 * @return r1 relational value.
 */

static int relation (void)
{
  int r1, r2, op;
  r1 = expression();
  op = tokenizer_token();
  while (op == T_EQUAL ||
  op == T_LT ||
  op == T_GT ||
  op == T_LT_EQ ||
  op == T_GT_EQ ||
  op == T_NOT_EQUAL)
  {
    tokenizer_next();
    r2 = expression();
    switch (op)
    {
      case T_EQUAL:
        r1 = r1 == r2;
        break;
      case T_LT:
        r1 = r1 < r2;
        break;
      case T_GT:
        r1 = r1 > r2;
        break;
      case T_LT_EQ:
        r1 = r1 <= r2;
        break;
      case T_GT_EQ:
        r1 = r1 >= r2;
        break;
      case T_NOT_EQUAL:
        r1 = r1 != r2;
        break;
    }
    op = tokenizer_token();
  }
  return r1;
}

/**
 * find_linenum
 *
 * @param linenum Line number to search and find.
 * @return void
 */

static void find_linenum (int linenum)
{
  /* Skip irrelevant new-lines and comments. */
  while (tokenizer_token() != T_NUMBER)
    tokenizer_next();
  while (tokenizer_num() != linenum &&
  tokenizer_token() != T_EOF)
  {
    /* Check line number, then skip the rest of line statement. */
    do {
      do {
        tokenizer_next();
      } while (tokenizer_token() != T_EOL &&
      tokenizer_token() != T_EOF);
      if (tokenizer_token() == T_EOL)
      {
        /* Skip irrelevant new-lines and comments. */
        while (tokenizer_token() != T_NUMBER &&
        tokenizer_token() != T_EOF)
          tokenizer_next();
      }
    } while (tokenizer_token() != T_NUMBER &&
      tokenizer_token() != T_EOF);
  }
}

/**
 * jump_linenum
 *
 * @param linenum The line number to [attempt] jump to.
 * @return void
 */

static void jump_linenum (int linenum)
{
  FILE *original;
  int   finished;

  /* Not only does this verify whether the scanner finished,
     if it has finished, it additionally closes the stream. */
  finished = tokenizer_finished();
  /* We save this copy in case the scanner wasn't finished. */
  original = io_handle();

  /* Start a new scanner from the beginning of the file. */
  tokenizer_init(io_file());
  reset(T_ERROR);
  io_reset();

  /* Search for linenum. */
  find_linenum(linenum);

  /* If the search ended at EOF, linenum could not be found! */
  if (tokenizer_finished())
  {
    dprintf(
      "*warning: could not jump to `%d'\n",
      E_WARNING, linenum);
    /* Set back to original stream */
    io_set(io_file(), original);
    /* Prepare scanner to continue. */
    if (!finished)
    {
      reset(T_NUMBER);
      io_reset();
      io_next();
    }
  }
}

/**
 * goto_statement
 *
 * @param void
 * @return void
 */

static void goto_statement (void)
{
  int to;
  accept(T_GOTO);
  to = tokenizer_num();
  accept(T_NUMBER);
  accept(T_EOL);
  jump_linenum(to);
}

/**
 * print_statement
 *
 * @param void
 * @return void
 */

static void print_statement (void)
{
  accept(T_PRINT);
  do {
    /* Print a string literal. */
    if (tokenizer_token() == T_STRING)
    {
      printf("%s", tokenizer_string());
      tokenizer_next();
    }
    /* A seperator, send a space. */
    else if (tokenizer_token() == T_SEPERATOR)
    {
      printf(" ");
      tokenizer_next();
    }
    /* Evaluate and print an expression. */
    else if (tokenizer_token() == T_LETTER ||
    tokenizer_token() == T_NUMBER ||
    tokenizer_token() == T_LEFT_PAREN)
      printf("%d", expression());
    else
    {
      break;
    }
    /* This additionally ensures a new-line character
       is present at the end of the line-statement. */
    if (tokenizer_finished())
      accept(T_EOL);
  } while (tokenizer_token() != T_EOL &&
    tokenizer_token() != T_EOF);

  printf("\n");
  tokenizer_next();
}

/**
 * if_statement
 *
 * @param void
 * @return void
 */

static void if_statement (void)
{
  int r, to;
  accept(T_IF);
  r = relation();
  accept(T_THEN);
  to = tokenizer_num();
  accept(T_NUMBER);
  accept(T_EOL);
  if (r)
    jump_linenum(to);
}

/**
 * let_statement
 *
 * @param void
 * @return void
 */

static void let_statement (void)
{
  int var;
  var = tokenizer_variable_num();
  accept(T_LETTER);
  accept(T_EQUAL);
  set_variable(var, expression());
  accept(T_EOL);
}

/**
 * statement
 *
 * @param void
 * @return void
 */

static void statement (void)
{
  int token;
  char string[10];
  token = tokenizer_token();
  switch (token)
  {
    /* REM statement (comment). */
    case T_REM:
      tokenizer_next();
      accept(T_EOL);
      break;
    /* Print statement. */
    case T_PRINT:
      print_statement();
      break;
    /* If statement. */
    case T_IF:
      if_statement();
      break;
    /* Goto statement. */
    case T_GOTO:
      goto_statement();
      break;
    /* Let statement. */
    case T_LET:
      accept(T_LET);
    /* Fall through... */
    case T_LETTER:
      let_statement();
      break;
    default:
    /* Unrecognized statement! */
      to_string(string, sizeof string);
      dprintf("*vvtbi.c: statement(): "
        "not implemented near `%s'\n",
        E_ERROR,
        /* If empty, EOF! */
        ((strlen(string)) ? string : "EOF"));
      break;
  }
}

/**
 * line_statement
 *
 * @param void
 * @return void
 */

static void line_statement (void)
{
  int token;
  /* Skip irrelevant new-lines. */
  if (tokenizer_token() == T_EOL)
  {
    do {
      tokenizer_next();
    } while (tokenizer_token() == T_EOL);
  }
  token = tokenizer_token();
  /* Unless a comment, line number is mandatory. */
  if (token != T_REM)
  {
    accept(T_NUMBER);
  }
  statement();
}

/**
 * vvtbi_run
 *
 * @param void
 * @return void
 */

void vvtbi_run (void)
{
  if (tokenizer_finished())
    return;
  /* interpret line-statements! */
  line_statement();
}

/**
 * vvtbi_finished
 *
 * @param void
 * @return void
 */

int vvtbi_finished (void)
{
  /* The interpreter is finished! */
  return tokenizer_finished();
}