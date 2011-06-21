#ifndef _IO_H__
#define _IO_H__

void io_init (const char *fl);
int  io_current (void);
void io_next (void);
void io_back (void);
void io_reset (void);
void io_seek (long offset, int whence);
long io_location (void);
int io_peek (void);
int  io_eof (void);
const char *io_file (void);
void io_set(const char *fl,
  FILE *handle);
void to_string (char *dest, size_t n);
void io_close (void);

#endif