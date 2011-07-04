/**********************************
   io.h, @format.new-line  lf
         @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
***********************************/
#ifndef _IO_H__
#define _IO_H__

void        io_init     (const char *filename);
int         io_current  (void);
void        io_next     (void);
void        io_back     (void);
void        io_reset    (void);
void        io_seek     (long offset, int whence);
long        io_location (void);
int         io_peek     (void);
int         io_eof      (void);
const char *io_file     (void);
void        io_set      (const char *f, FILE *handle);
FILE       *io_handle   (void);
void        to_string   (char *dest, size_t n);
void        io_close    (void);

#endif /* _IO_H__ */