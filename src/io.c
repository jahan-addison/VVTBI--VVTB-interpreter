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

/* The API file location. */
static const char *file;

/* The API file-handle. */
static FILE *Handle = NULL;

/* The API current character in stream. */
static int current  = 0;

/* The next character in stream. */
static int next     = 0;

/******************************************************************************/

/**
 * io_init
 *
 * @param fl The initial file for the API.
 * @return void
 */

void io_init (const char *filename)
{
  file   = filename;
  Handle = fopen(file, "r");
  /* Fopen failed! */
  if (!Handle)
  {
    fprintf(stderr,
      "*io.c: file `%s' failed!\n",
      file);
    /* Terminate program. */
    exit(EXIT_FAILURE);
  }
  io_next();
}

/**
 * io_current
 *
 * @param void
 * @return current The current character.
 */

int io_current (void)
{
  return current;
}

/**
 * io_reset
 *
 * @param void
 * @return void
 */

void io_reset (void)
{
  current = 0;
}

/**
 * io_next
 *
 * @param void
 * @return void
 */

void io_next (void)
{
  /* If io_next hasn't been used, set both current and next. */
  if (!current)
  {
    current = getc(Handle);
    next    = getc(Handle);
  }
  /* Otherwise, set current to next and retrieve the next character. */
  else
  {
    current = next;
    next    = getc(Handle);
  }
}

/**
 * io_peek
 *
 * @param void
 * @return next The next character in stream.
 */

int io_peek (void)
{
  return next;
}

/**
 * io_location
 *
 * @param void
 * @return ftell
 */

long io_location (void)
{
  return ftell(Handle);
}

/**
 * io_seek
 *
 * @param offset The offset in the file stream.
 * @param whence The initial location for offset.
 * @return fseek
 */

void io_seek (long offset, int whence)
{
  fseek(Handle, offset, whence);

}

/**
 * io_eof
 *
 * @param void
 * @return Equality of current and EOF.
 */

int io_eof (void)
{
  return current == EOF;
}

/**
 * to_string
 *
 * @param dest The destination to copy characters.
 * @param n The max amount of characters to copy.
 * @return void
 */

void to_string (char *dest, size_t n)
{
  size_t i;
  i = 0;
  while (n)
  {
    /* EOF! Abort! */
    if (io_eof())
      break;
    dest[i++] = io_current();
    io_next();
    n--;
  }
  dest[i] = 0;
}

/**
 * io_set
 *
 * @param f File to update API.
 * @param handle File-handle to update API.
 * @return void
 */

void io_set (const char *filename, FILE *handle)
{
  /* Close the current stream. */
  if (Handle)
    io_close();
  file   = filename;
  Handle = handle;
}

/**
 * io_handle
 *
 * @param void
 * @return Handle The current file-handle object.
 */

FILE *io_handle (void)
{
  return Handle;
}

/**
 * io_file
 *
 * @param void
 * @return file The current file.
 */

const char *io_file (void)
{
  return file;
}

/**
 * io_close
 *
 * @param void
 * @return void
 */

void io_close (void)
{
  fclose(Handle);
}