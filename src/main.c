/*********************************
   main.c, @format.new-line  lf
           @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
**********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "tokenizer.h"
#include "vvtbi.h"

/* Vvtbi's version number. */
#define VERSION "2.0"

/* The message printed if no arguments are given. */
#define NOARGS  "VERSION: " VERSION "\n"      \
  "***************************************\n" \
  "  Howto: ./vvtbi [-debug] file."           \
  VVTBI_EXTENSION_LITERAL "\n"

/******************************************************************************/

/**
 * io_init
 *
 * @param filename File to check against.
 * @return Filename ends with vvtbi extension.
 */

int valid(const char *filename)
{
  const char *end;
  end = strrchr(filename, '.');
  /* Is the source file O.K? */
  return (end != NULL) && (strcmp(end + 1,
    VVTBI_EXTENSION_LITERAL) == 0);
}

/******************/
/* Start program. */
/******************/

int main (int argc, char **argv)
{
  if (argc < 2)
  {
    /* No args, print message. */
    printf(NOARGS);
  }
  /* Debug mode, run and print scanner only. */
  else if (!strcmp(argv[1], "-debug"))
  {
    if (argc < 3)
    {
      printf(NOARGS);
    }
    else
    {
      /* Check file type. */
      if (!valid(argv[2])) return EXIT_FAILURE;

      tokenizer_init(argv[2]);
      /* Run scanner until EOF. */
      do {
        /* Print token string. */
        printf("%s ", vvtbi_token(tokenizer_token()));
        if (tokenizer_token() == T_EOL)
          printf("\n");
        tokenizer_next();
      } while (!tokenizer_finished());
    }
  }
  /* Run interpreter. */
  else
  {
    /* Check file type. */
    if (!valid(argv[1])) return EXIT_FAILURE;

    vvtbi_init(argv[1]);
    /* Run interpreter until EOF. */
    do {
      vvtbi_run();
    } while (!vvtbi_finished());

  }
  /* Complete! :) */
  return EXIT_SUCCESS;
}

/********/
/* End. */
/********/