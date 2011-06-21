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

/* I/O file */
static const char *file;

/* I/O FILE stream */
static FILE *Handle = NULL;

/* current characters in stream */
static int current = 0, next = 0;

/******************************************************************************/

/* initialize I/O */

void io_init (const char *fl)
{
  file   = fl;
  Handle = fopen(file, "rb");
  if (!Handle)
  {
    /* failed! */
    fprintf(stderr,
  "*io.c: file `%s' failed!\n",
    file);
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

/* reset current character */

void io_reset (void)
{
  current = 0;
}

/* set to next character in stream */

void io_next (void)
{
  if (!current)
  {
    current = getc(Handle);
    /* save a copy of the next */
    next    = getc(Handle);
  }
  else
  {
  /* set next to current, continue */
    current = next;
    next    = getc(Handle);
  }
}

/* peek the next character in stream */

int io_peek (void)
{
  return next;
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

/* set to new stream */

void io_set(const char *fl,
  FILE *handle)
{
  if (Handle)
    io_close();
  file   = fl;
  Handle = handle;
}

/* file */

const char *io_file (void)
{
  return file;
}

/* close file stream */

void io_close (void)
{
  fclose(Handle);
}