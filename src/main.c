/*********************************
   main.c, @format.new-line  lf
           @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
**********************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION "1.2"

#include "io.h"
#include "tokenizer.h"
#include "vvtbi.h"

/******************************************************************************/

/*********************/
/* start our program */
/*********************/

int main (int argc, char **argv)
{
  FILE *program;
  if (argc < 2)
  {
    printf("***\n"
      "@version: %s\n"
      "********************************\n"
      "  Howto: ./vvtbi[.exe] file\n\n",
      VERSION
    );
  }
  else
  {
    if (!(program = fopen(argv[1], "rb")))
    {
      fprintf(stderr, "**main.c: file not found!\n");
      exit(EXIT_FAILURE);
    }
    /**********************/
    /* start interpreter! */
    /**********************/
    vvtbi_init(program);
    do {
      vvtbi_run();
    } while (!vvtbi_finished());
  }
  /*********/
  /* done! */
  /*********/
  return 0;
}

/*******/
/* End */
/*******/