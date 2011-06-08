#ifndef _VVTBI_H__
#define _VVTBI_H__

void vvtbi_init (const char *source);
void vvtbi_run (void);
int vvtbi_finished (void);
int get_variable (int place);
void set_variable (int place, int value);

#endif