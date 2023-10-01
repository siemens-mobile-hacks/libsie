#ifndef __SIE_FILES_H__
#define __SIE_FILES_H__

typedef struct {
    void *prev;
    void *next;
    char *file_name;
    short file_attr;
} SIE_FILE;

SIE_FILE *Sie_Files_Find(const char *mask);
void Sie_Files_Destroy(SIE_FILE *top);

unsigned int Sie_Files_GetCount(SIE_FILE *top);
SIE_FILE *Sie_Files_GetFileByID(SIE_FILE *top, unsigned int id);
SIE_FILE *Sie_Files_GetFileByFileName(SIE_FILE *top, const char *file_name);

SIE_FILE *Sie_Files_Sort(SIE_FILE *top, int cmp(SIE_FILE*, SIE_FILE*), int keep_folders_on_top);
SIE_FILE *Sie_Files_SortByName(SIE_FILE *top, int keep_folders_on_top);

#endif
