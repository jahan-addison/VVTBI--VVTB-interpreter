/**********************************
   vvtbi.c, @format.new-line  lf
            @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
***********************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "io.h"
#include "tokenizer.h"
#include "vvtbi.h"

#define MAX_STRING_LEN   50
#define MAX_NUMBER_LEN   6
#define MAX_VARIABLE_NUM 26 /* a - z */

/* error constants */
typedef enum {
  E_WARNING,
  E_ERROR
} Error;

/* strings of scanner tokens */
static const char *token_stringify[] = {
  NULL, /* skip 0 index */
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

/* variable container */
static int variables[MAX_VARIABLE_NUM];

static int expression (void);
static void line_statement (void);
static void statement (void);

/******************************************************************************/

/* debugging routine */

void dprintf(const char *format,
  Error error, ...)
{
  va_list args;

  va_start(args, error);
  vfprintf(stderr, format, args);
  va_end(args);
  if (error == E_ERROR)
  {
    exit(EXIT_FAILURE);
  }
}

/* initialize tokenizer */

void vvtbi_init (const char *source)
{
  tokenizer_init(source);
  /* initialize variable container */
  memset(variables, 0, MAX_VARIABLE_NUM);
}

/* accept a token from the scanner */

static void accept (int token)
{
  char nearby[10];
  if (token != tokenizer_token())
  {
    to_string(nearby, sizeof nearby);
    dprintf("*vvtbi.c: "
      "unexpected `%s' near `%s',"
      " expected: `%s'\n",
      E_ERROR,
      token_stringify[tokenizer_token()],
      nearby,
      token_stringify[token]);
  }
  tokenizer_next();
}

/* factor necessary components */

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
        tokenizer_variable_num()
      );
      accept(T_LETTER);
      break;
  }
  return r;
}

/* parse term */

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
          /* divide by zero */
          dprintf("*warning: "
            "divide by zero!\n",
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

/* parse expression */

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

/* parse relation */

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

/* search file for line number */

static void goto_line (int number)
{
  char linenum[MAX_NUMBER_LEN+1];
  FILE *search;
  size_t i           = 0;
  int num            = 0, c;
  search             = fopen(io_file(),
    "r");
  while(1)
  {
    c                = getc(search);
    if (c == EOF) break;
    /* save line number and verify */
    if (isdigit(c))
    {
      linenum[i++]   = c;
      while ((c      = getc(search)))
      {
        if (!isdigit(c))
          break;
        if (i > sizeof linenum)
          break;
        linenum[i++] = c;
      }
      linenum[i]     = 0;
      num            = strtol(linenum,
        NULL, 0);
      if (number == num)
        break;
    }

    /* skip until EOL */
    while (c != '\n')
      c = getc(search);
    /* start again */
    i                = 0;
  }
  /* matched! */
  if (num == number)
  {
    /* set (close current) */
    io_set(io_file(), search);
    reset(T_NUMBER);
    io_reset(); io_next();
    /* finished! */
    return;
  }
  /* failed! */
  else
  {
    dprintf("*warning: "
      "could not jump to line `%d'\n",
      E_WARNING,
      number
    );
  }
}

/* parse goto statement */

static void goto_statement (void)
{
  int to; accept(T_GOTO);
  to = tokenizer_num();
  accept(T_NUMBER); accept(T_EOL);
  goto_line(to);
}

/* parse print statement */

static void print_statement (void)
{
  accept(T_PRINT);
  do {
    /* begin parsing print statement segments */
    if (tokenizer_token() == T_STRING)
    {
      printf("%s", tokenizer_string());
      tokenizer_next();
    }
    else if (tokenizer_token() == T_SEPERATOR)
    {
      printf(" ");
      tokenizer_next();
    }
    else if (tokenizer_token() == T_LETTER ||
    tokenizer_token() == T_NUMBER ||
    tokenizer_token() == T_LEFT_PAREN)
    {
      printf("%d", expression());
    }
    else
    {
      break;
    }
    /* last character must be newline */
    if (tokenizer_finished())
    {
      accept(T_EOL);
    }
  } while (tokenizer_token() != T_EOL &&
    tokenizer_token() != T_EOF);
  printf("\n");
  tokenizer_next();
}

/* parse if statement */

static void if_statement (void)
{
  int r, to;
  accept(T_IF); r = relation();
  accept(T_THEN); to = tokenizer_num();
  accept(T_NUMBER); accept(T_EOL);
  if (r) goto_line(to);
}

/* parse let statement */

static void let_statement (void)
{
  int var;
  var = tokenizer_variable_num();
  accept(T_LETTER); accept(T_EQUAL);
  set_variable(var, expression());
  accept(T_EOL);
}

/* parse statement */

static void statement (void)
{
  int token;
  char nearby[10];
  token = tokenizer_token();
  switch (token)
  {
    case T_REM:
      tokenizer_next();
      accept(T_EOL);
      break;
    case T_PRINT:
      print_statement();
      break;
    case T_IF:
      if_statement();
      break;
    case T_GOTO:
      goto_statement();
      break;
    case T_LET:
      accept(T_LET);
      /* fall through */
    case T_LETTER:
      let_statement();
      break;
    default:
      /* not implemented! */
      to_string(nearby, sizeof nearby);
      dprintf("*vvtbi.c: statement(): "
        "not implemented near `%s'\n",
        E_ERROR,
        nearby);
      break;
  }
}

/* parse line statement */

static void line_statement (void)
{
  int token;
  /* skip blank lines */
  if (tokenizer_token() == T_EOL)
  {
    do {
      tokenizer_next();
    } while (tokenizer_token() == T_EOL);
  }
  token = tokenizer_token();
  if (token != T_REM)
  {
    accept(T_NUMBER);
  }
  statement();
}

/* parse line statement */

void vvtbi_run (void)
{
  if (tokenizer_finished())
    return;
  line_statement();
}

/* check if scanner is complete */

int vvtbi_finished (void)
{
  return tokenizer_finished();
}

/* set and store variable */

void set_variable (int place, int value)
{
  if (place >= 0 && place <= MAX_VARIABLE_NUM)
    variables[place] = value;
}

/* get variable from container */

int get_variable (int place)
{
  if (place >= 0 && place <= MAX_VARIABLE_NUM)
    return variables[place];
  return 0;
}