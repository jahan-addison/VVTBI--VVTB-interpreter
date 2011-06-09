/********************************
   io.c, @format.new-line  lf
         @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
*********************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"

/*******************/
/* I/O File stream */
/*******************/
static FILE *Handle;

/*******************************/
/* current character in stream */
/*******************************/
static int current = 0;

/******************************************************************************/

/* initialize I/O */

void io_init (const char *fl)
{
  Handle = fopen(fl, "rb");
  if (!Handle)
  {
    /* failed! */
    fprintf(stderr,
  "*io.c: file `%s' failed!\n",
    fl);
    /* exit! */
    exit(EXIT_FAILURE);
  }
  io_next();
}

/* last read character in stream */

int io_current (void)
{
  return current;
}

/* set to next character in stream */

void io_next (void)
{
  current = getc(Handle);
}

/* peek the next character in stream */

int io_peek (void)
{
  int next;  next = getc(Handle);
  io_seek(-1, SEEK_CUR); return next;
}

/* return current position in file */

long io_location (void)
{
  return ftell(Handle);
}

/* set to a new position in file */

void io_seek (long offset, int whence)
{
  fseek(Handle, offset, whence);

}

/* return is stream EOF */

int io_eof (void)
{
  return current == EOF;
}

/* set string dest to n chars in stream */

void to_string (char *dest, size_t n)
{
  size_t i;
  for (i = 0; !io_eof(); i++)
  {
    if (i == n) break;
    dest[i] = io_current();
    io_next();
  }
  dest[i] = 0;
}

/* close file stream */

void io_close (void)
{
  fclose(Handle);
}