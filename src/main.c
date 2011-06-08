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
  if (argc < 2)
  {
    printf("***\r\n"
      "@version: %s\r\n"
      "********************************\r\n"
      "  Howto: ./vvtbi[.exe] file\r\n",
      VERSION
    );
  }
  else
  {
    /**********************/
    /* start interpreter! */
    /**********************/
    vvtbi_init(argv[1]);
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