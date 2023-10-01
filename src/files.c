#include <swilib.h>
#include <stdlib.h>
#include "include/sie/files.h"

SIE_FILE *Sie_Files_Find(const char *mask) {
    SIE_FILE *top = NULL;
    SIE_FILE *current = NULL;
    SIE_FILE *prev = NULL;

    DIR_ENTRY de;
    unsigned int err;
    if (FindFirstFile(&de, mask, &err)) {
        do {
            size_t len;

            current = malloc(sizeof(SIE_FILE));
            zeromem(current, sizeof(SIE_FILE));

            // file_name
            len = strlen(de.file_name);
            current->file_name = malloc(sizeof(char) * (len + 1));
            strcpy(current->file_name, de.file_name);
            // file_attr
            current->file_attr = de.file_attr;
            if (!prev) {
                top = current;
            } else {
                prev->next = current;
                current->prev = prev;
            }
            prev = current;
        } while (FindNextFile(&de, &err));
        FindClose(&de, &err);
    }
    return top;
}

void Sie_Files_Destroy(SIE_FILE *top) {
    SIE_FILE *p = top;
    while (p) {
        SIE_FILE *next = p->next;
        mfree(p->file_name);
        mfree(p);
        p = next;
    }
}

unsigned int Sie_Files_GetCount(SIE_FILE *top) {
    unsigned int count = 0;
    while (top) {
        top = top->next;
        count++;
    }
    return count;
}

SIE_FILE *Sie_Files_GetFileByID(SIE_FILE *top, unsigned int id) {
    SIE_FILE *file = NULL;
    unsigned int i = 0;
    while (top) {
        if (i == id) {
            file = top;
            break;
        }
        top = top->next;
        i++;
    }
    return file;
}

SIE_FILE *Sie_Files_GetFileByFileName(SIE_FILE *top, const char *file_name) {
    SIE_FILE *file = NULL;
    while (top) {
        if (strcmp(top->file_name, file_name) == 0) {
            file = top;
            break;
        }
        top = top->next;
    }
    return file;
}

void Sie_Files_DeleteElement(SIE_FILE *top, SIE_FILE *element) {
    SIE_FILE *p = top;
    while (p) {
        if (p == element) {
            SIE_FILE *prev = element->prev;
            SIE_FILE *next = element->next;
            if (prev) {
                prev->next = next;
            }
            if (next) {
                next->prev = prev;
            }
            mfree(element);
            break;
        }
        p = p->next;
    }
}

SIE_FILE *Sie_Files_Sort(SIE_FILE *top, int cmp(SIE_FILE*, SIE_FILE*), int keep_folders_on_top) {
    SIE_FILE *new_top = NULL;
    SIE_FILE *prev = NULL;
    SIE_FILE *current = NULL;

    SIE_FILE *p = NULL;
    SIE_FILE *p2 = NULL;

    p = top;
    while (p) {
        SIE_FILE *found = p;
        p2 = p->next;
        while (p2) {
            if (cmp(p2, found)) {
                found = p2;
            }
            p2 = p2->next;
        }
        current = malloc(sizeof(SIE_FILE));
        memcpy(current, found, sizeof(SIE_FILE));
        Sie_Files_DeleteElement(p, found);
        if (found == p) {
            p = p->next;
        }

        current->prev = prev;
        current->next = NULL;
        if (!prev) {
            new_top = current;
        } else {
            prev->next = current;
        }
        prev = current;
    }

    if (keep_folders_on_top) {
        SIE_FILE *top_dir = NULL;
        SIE_FILE *top_file = NULL;
        SIE_FILE *last_dir = NULL;
        SIE_FILE *last_file = NULL;
        p = new_top;
        while (p) {
            if (p->file_attr & FA_DIRECTORY) {
                SIE_FILE *current_dir = malloc(sizeof(SIE_FILE));
                memcpy(current_dir, p, sizeof(SIE_FILE));
                Sie_Files_DeleteElement(p, p);
                current_dir->next = NULL;
                current_dir->prev = last_dir;
                if (!last_dir) {
                    top_dir = current_dir;
                } else {
                    last_dir->next = current_dir;
                }
                last_dir = current_dir;
            } else {
                SIE_FILE *current_file = malloc(sizeof(SIE_FILE));
                memcpy(current_file, p, sizeof(SIE_FILE));
                Sie_Files_DeleteElement(p, p);
                current_file->next = NULL;
                current_file->prev = last_file;
                if (!last_file) {
                    top_file = current_file;
                } else {
                    last_file->next = current_file;
                }
                last_file = current_file;
            }
            p = p->next;
        }
        if (top_dir) {
            last_dir->next = top_file;
        }
        if (top_file) {
            top_file->prev = last_dir;
        }
        new_top = (top_dir) ? top_dir : top_file;
    }

    return new_top;
}

SIE_FILE *Sie_Files_SortByName(SIE_FILE *top, int keep_folders_on_top) {
    int cmp(SIE_FILE *f1, SIE_FILE *f2) {
        return (strcmpi(f1->file_name, f2->file_name) < 0);
    }
    return Sie_Files_Sort(top, cmp, keep_folders_on_top);
}
