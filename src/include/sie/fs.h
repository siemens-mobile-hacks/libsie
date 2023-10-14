#ifndef __SIE_FILES_H__
#define __SIE_FILES_H__

typedef struct {
    void *prev;
    void *next;
    char *dir_name;
    char *file_name;
    short file_attr;
} SIE_FILE;

SIE_FILE *Sie_FS_FindFiles(const char *mask);
SIE_FILE *Sie_FS_FindFilesRecursive(const char *mask);
void Sie_FS_DestroyFiles(SIE_FILE *top);

unsigned int Sie_FS_GetFilesCount(SIE_FILE *top);
SIE_FILE *Sie_FS_GetFileByID(SIE_FILE *top, unsigned int id);
SIE_FILE *Sie_FS_GetFileByFileName(SIE_FILE *top, const char *file_name);
SIE_FILE *Sie_FS_GetLastFile(SIE_FILE *top);
char *Sie_FS_GetPathByFile(SIE_FILE *file);

SIE_FILE *Sie_FS_SortFiles(SIE_FILE *top, int cmp(SIE_FILE*, SIE_FILE*), int keep_folders_on_top);
SIE_FILE *Sie_FS_SortFilesByName(SIE_FILE *top, int keep_folders_on_top);

int Sie_FS_RemoveDirRecursive(const char *dir);

#endif
