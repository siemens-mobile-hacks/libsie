#include <swilib.h>
#include "../include/sie/fs/fs.h"

SIE_FILE *Sie_FS_SortFiles(SIE_FILE *files, int cmp(SIE_FILE*, SIE_FILE*), int keep_dirs_on_top) {
    SIE_FILE *p = files;
    SIE_FILE *prev = NULL;
    SIE_FILE *next = NULL;
    SIE_FILE *new_files = NULL;
    while (p) {
        SIE_FILE *found = p;
        SIE_FILE *p2 = p->next;
        next = p->next;
        while (p2) {
            if (cmp(p2, found)) {
                found = p2;
            }
            p2 = p2->next;
        }
        if (p != found) {
            SIE_FILE *found_prev = found->prev;
            SIE_FILE *found_next = found->next;
            if (found_prev) {
                found_prev->next = found_next;
            }
            if (found_next) {
                found_next->prev = found_prev;
            }
            found->prev = NULL;
            found->next = NULL;
        }
        if (!new_files) {
            new_files = found;
            new_files->prev = NULL;
            new_files->next = NULL;
        } else {
            prev->next = found;
            found->prev = prev;
        }
        prev = found;
        if (p == found) {
            p = next;
        }
    }

    if (keep_dirs_on_top) {
        SIE_FILE *dirs = NULL;
        SIE_FILE *last_dir = NULL;
        SIE_FILE *first_file = NULL;
        p = new_files;
        while (p) {
            prev = p->prev, next = p->next;
            if (p->file_attr & SIE_FS_FA_DIRECTORY) {
                SIE_FILE *dir = Sie_FS_CopyFileElement(p);
                if (prev) {
                    prev->next = next;
                }
                if (next) {
                    next->prev = prev;
                }
                Sie_FS_DestroyFileElement(p);
                if (!dirs) {
                    dirs = dir;
                } else {
                    last_dir->next = dir;
                    dir->prev = last_dir;
                }
                last_dir = dir;
            }
            else if (!first_file) {
                first_file = p;
            }
            p = next;
        }
        if (last_dir) {
            if (first_file) {
                last_dir->next = first_file;
                first_file->prev = last_dir;
            }
            return dirs;
        }
    }
    return new_files;
}


/**********************************************************************************************************************/

static int CompareByNameAsc(SIE_FILE *f1, SIE_FILE *f2) {
    return (strcmpi(f1->file_name, f2->file_name) < 0);
}

SIE_FILE *Sie_FS_SortFilesByNameAsc(SIE_FILE *files, int keep_dirs_on_top) {
    return Sie_FS_SortFiles(files, CompareByNameAsc, keep_dirs_on_top);
}

/**********************************************************************************************************************/

