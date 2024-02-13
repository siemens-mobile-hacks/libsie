#ifndef __SIE_EXEC_H__
#define __SIE_EXEC_H__

int Sie_Exec_File(const char *path);
int Sie_Exec_Shortcut(const char *shortcut);
int Sie_Exec_Entrypoint(const char *entrypoint);
int Sie_Exec_Execute(const char *param);

#endif
