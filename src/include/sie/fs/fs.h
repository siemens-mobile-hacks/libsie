#ifndef __SIE_FS_H__
#define __SIE_FS_H__

#include <swilib.h>

#define SIE_FS_FA_READONLY  FA_READONLY
#define SIE_FS_FA_HIDDEN    FA_HIDDEN
#define SIE_FS_FA_SYSTEM    FA_SYSTEM
#define SIE_FS_FA_VOLUME    0x0008
#define SIE_FS_FA_DIRECTORY FA_DIRECTORY
#define SIE_FS_FA_ARCHIVE   FA_ARCHIVE

typedef struct {
    void *prev;
    void *next;
    char *dir_name;
    char *file_name;
    char *alias;
    short file_attr;
} SIE_FILE;

SIE_FILE *Sie_FS_FindFiles(const char *mask);
SIE_FILE *Sie_FS_FindFilesRecursive(const char *mask);
SIE_FILE *Sie_FS_CloneFiles(SIE_FILE *top);
void Sie_FS_DestroyFiles(SIE_FILE *top);

unsigned int Sie_FS_ContainsFile(SIE_FILE *top, SIE_FILE *file);
unsigned int Sie_FS_GetFilesCount(SIE_FILE *top);
SIE_FILE *Sie_FS_GetFirstFile(SIE_FILE *top);
SIE_FILE *Sie_FS_GetLastFile(SIE_FILE *top);
SIE_FILE *Sie_FS_GetFileByID(SIE_FILE *top, unsigned int id);
SIE_FILE *Sie_FS_GetFileByFileName(SIE_FILE *top, const char *file_name);
SIE_FILE *Sie_FS_GetFileByAlias(SIE_FILE *top, const char *alias);
SIE_FILE *Sie_FS_GetUniqueFile(SIE_FILE *file);
char *Sie_FS_GetPathByFile(SIE_FILE *file);

SIE_FILE *Sie_FS_CopyFileElement(SIE_FILE *file);
void Sie_FS_DestroyFileElement(SIE_FILE *file);
SIE_FILE *Sie_FS_DeleteFileElement(SIE_FILE *top, SIE_FILE *file);

SIE_FILE *Sie_FS_SortFiles(SIE_FILE *files, int cmp(SIE_FILE*, SIE_FILE*), int keep_dirs_on_top);
SIE_FILE *Sie_FS_SortFilesByNameAsc(SIE_FILE *files, int keep_dirs_on_top);
SIE_FILE *Sie_FS_SortFilesByNameDesc(SIE_FILE *files, int keep_dirs_on_top);
SIE_FILE *Sie_FS_ExcludeFilesByFileAttr(SIE_FILE *top, short file_attr);

unsigned int Sie_FS_FileExists(const char *path);
unsigned int Sie_FS_MMCardExists();
unsigned int Sie_FS_IsDir(const char *path, unsigned int *err);

unsigned int Sie_FS_CreateFile(const char *path, unsigned int *err);
unsigned int Sie_FS_CreateDir(const char *path, unsigned int *err);
unsigned int Sie_FS_CreateDirs(const char *path, unsigned int *err);
unsigned int Sie_FS_CopyFile(const char *src, const char *dest, unsigned int *err);
unsigned int Sie_FS_CopyDir(const char *src, const char *dest, unsigned int *err);
unsigned int Sie_FS_MoveFile(const char *src, const char *dest, unsigned int *err);
unsigned int Sie_FS_DeleteFile(const char *path, unsigned int *err);
unsigned int Sie_FS_DeleteDir(const char *path, unsigned int *err);
unsigned Sie_FS_DeleteDirRecursive(const char *path, unsigned int *err);

#endif
