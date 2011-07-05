/**********************************
   vvtbi.h, @format.new-line  lf
            @format.use-tabs  false
   @format.tab-size    2
   @format.indent-size 2
   @format.line-length 80
***********************************/
#ifndef _VVTBI_H__
#define _VVTBI_H__

void        vvtbi_init     (const char *source);
void        vvtbi_run      (void);
const char *vvtbi_token    (int token);
int         vvtbi_finished (void);

#endif /* _VVTBI_H__ */