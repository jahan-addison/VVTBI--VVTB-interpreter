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

#include "config.h"
#include "io.h"
#include "tokenizer.h"

/* The last token scanned. */
static int token;

/* The scanner's data "pointer." */
union Pointer {
  char string[VVTBI_STRING_LITERAL+1];
  int  number;
  int  letter;
} text;

struct keyword_token {
  char *keyword;
  int   token;
};

/* Language keywords. */
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

/**
 * tokenizer_init
 *
 * @param source The source code file.
 * @return void
 */

void tokenizer_init (const char *source)
{
  io_init(source);
  token = get_next_token();
}

/**
 * token_relation
 *
 * @param void
 * @return token A relational token.
 */

static int token_relation (void)
{
  int token;
  token = 0;
  switch (io_current())
  {
    /* The equal token. */
    case '=':
      io_next();
      token = T_EQUAL;
      break;
    case '<':
      io_next();
      /* The unequal token. */
      if (io_peek() == '>')
      {
        io_next();
        token = T_NOT_EQUAL;
      }
      /* The less-than-or-equal-to token. */
      else if (io_peek() == '=')
      {
        io_next();
        token = T_LT_EQ;
      }
      /* The less-than token. */
      else
      {
        io_next();
        token = T_LT;
      }
      break;
      /* The greater-than token. */
    case '>':
      io_next();
      /* The greater-than-or-equal-to token. */
      if (io_peek() == '=')
      {
        io_next();
        return T_GT_EQ;
      }
      /* The greater-than token. */
      else
      {
        io_next();
        return T_GT;
      }
      break;
  }

  return token;
}

/**
 * token_operation
 *
 * @param void
 * @return token An arithmetic token.
 */

static int token_operation (void)
{
  int token;
  token = 0;
  switch (io_current())
  {
    case '(':
      token = T_LEFT_PAREN;
      break;
    case ')':
      token = T_RIGHT_PAREN;
      break;
    case '+':
      token = T_PLUS;
      break;
    case '-':
      token = T_MINUS;
      break;
    case '/':
      token = T_SLASH;
      break;
    case '*':
      token = T_ASTERISK;
      break;
  }
  if (token)
    io_next();
  return token;
}

/**
 * token_string
 *
 * @param void
 * @return token A string token.
 */

static int token_string (void)
{
  size_t i;
  i = 0;
  /* Skip the initial ". */
  io_next();
  while (io_current() != '"' &&
  io_current() != EOF)
  {
    if (i >= VVTBI_STRING_LITERAL)
      break;
    text.string[i++] = io_current();
    io_next();
  }
  /* Skip proceeding ". */
  io_next();

  text.string[i] = 0;

  return T_STRING;
}

/**
 * token_keyword
 *
 * @param void
 * @return token A keyword token.
 */

static int token_keyword (void)
{
  size_t                      i;
  int                         token;
  struct keyword_token const *kt;

  for (i = 0, token = 0, kt = keywords; kt->keyword; kt++)
  {
    /* A character in the stream matched a keyword character. */
    while (toupper(kt->keyword[i]) == io_current())
    {
      /* We pre-increment to check the next character, and if we've
         encountered the null-terminator, we have a match! */
      if (!(kt->keyword[++i]))
      {
        if (kt->token == T_REM)
        {
          /* For the particular REM keyword, skip all characters
             until EOL. */
          while(io_current() != '\r'
          && io_current() != '\n'
          && io_current() != EOF)
            io_next();
          token = T_REM;
          break;
        }
        /* Finished! Return token! */
        io_next();
        token = kt->token; break;
      }
      io_next();
    }
  }
  return token;
}

/**
 * token_number
 *
 * @param void
 * @return token A [whole] number token.
 */

static int token_number (void)
{
  size_t i;
  char   string[VVTBI_NUMBER_LITERAL+1];

  for (i = 0; i <= VVTBI_NUMBER_LITERAL; i++)
  {
    /* Stream no longer a number, we've finished. */
    if (!isdigit(io_current()))
    {
      string[i]   = 0;
      text.number = strtol(string, NULL, 10);
      return T_NUMBER;
    }
    string[i] = io_current();
    io_next();
  }
  /* The number exceeds VVTBI_NUMBER_LITERAL. */
  return T_ERROR;
}

/**
 * token_eol
 *
 * @param c The character in stream.
 * @return token The EOL token.
 */

static int token_eol (int c)
{
  if (c == '\r')
  {
    c = io_peek();
    io_next();
    /* EOL is \r\n. */
    if (c == '\n')
    {
      io_next();
      return T_EOL;
    }
    /* EOL is \r. */
    return T_EOL;
  }
  else if (c == '\n')
  {
    /* EOL is \n. */
    io_next();
    return T_EOL;
  }

  return 0;
}

/**
 * get_next_token
 *
 * @param void
 * @return token The next token in the scanner.
 */

static int get_next_token (void)
{
  int c, token;

  c = io_current();

  /* The EOF token. */
  if (c == EOF) return T_EOF;

  /* The seperator token. */
  if (c == ';' || c == ',')
  {
    io_next();
    return T_SEPERATOR;
  }

  /* The EOL token. */
  if (c == '\r' || c == '\n')
    return token_eol(c);

  /* A relational token. */
  token = token_relation();
  if (token)
    return token;

  /* An arithmetic token. */
  token = token_operation();
  if (token)
    return token;

  /* A keyword token. */
  token = token_keyword();
  if (token)
    return token;

  /* The string token. */
  if (c == '"')
    return token_string();

  /* The number token. */
  if (isdigit(c))
    return token_number();

  /* The letter (variable) token. */
  if (isalnum(c) && islower(c))
  {
    text.letter = c;
    io_next();
    return T_LETTER;
  }

  io_next();

  /* Scanned unrecognized data. */
  return T_ERROR;
}

/**
 * tokenizer_next
 *
 * @param void
 * @return void
 */

void tokenizer_next (void)
{
  if (tokenizer_finished())
    return;
  /* Skip whitespace. */
  while (io_current() == ' ' ||
  io_current() == '\t')
    io_next();
  token = get_next_token();
}

/**
 * tokenizer_token
 *
 * @param void
 * @return token Last token scanned.
 */

int tokenizer_token (void)
{
  return token;
}

/**
 * tokenizer_string
 *
 * @param void
 * @return text.string The pointer to string data.
 */

char *tokenizer_string(void)
{
  return text.string;
}

/**
 * tokenizer_num
 *
 * @param void
 * @return text.number The pointer to number data.
 */

int tokenizer_num (void)
{
  return text.number;
}

/**
 * tokenizer_variable_num
 *
 * @param void
 * @return A variable's corresponding cell location.
 */

int tokenizer_variable_num (void)
{
  /* This method will be used over text.letter - 'a'
     to prevent non-portable, ASCII-only, code. */
  switch (text.letter)
  {
    case 'a': return 0;
    case 'b': return 1;
    case 'c': return 2;
    case 'd': return 3;
    case 'e': return 4;
    case 'f': return 5;
    case 'g': return 6;
    case 'h': return 7;
    case 'i': return 8;
    case 'j': return 9;
    case 'k': return 10;
    case 'l': return 11;
    case 'm': return 12;
    case 'n': return 13;
    case 'o': return 14;
    case 'p': return 15;
    case 'q': return 16;
    case 'r': return 17;
    case 's': return 18;
    case 't': return 19;
    case 'u': return 20;
    case 'v': return 21;
    case 'w': return 22;
    case 'x': return 23;
    case 'y': return 24;
    case 'z': return 25;
  }
  /* Should not reach here. */
  return 0;
}

/**
 * reset
 *
 * @param to Token to set to.
 * @return void
 */

void reset (int to)
{
  token = to;
}

/**
 * tokenizer_finished
 *
 * @param void
 * @return Equality of current token and EOF token.
 */

int tokenizer_finished (void)
{
  /* If the scanner reached EOF, close file-handle. */
  if (token == T_EOF)
    io_close();
  return token == T_EOF;
}