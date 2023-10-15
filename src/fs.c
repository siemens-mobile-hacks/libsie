#include <swilib.h>
#include <stdlib.h>
#include "include/sie/fs.h"

#define COPY_BUFFER_SIZE 4096

SIE_FILE *Sie_FS_FindFiles(const char *mask) {
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
            // dir_name
            len = strlen(de.folder_name);
            current->dir_name = malloc(len + 1 + 1);
            sprintf(current->dir_name, "%s\\", de.folder_name);
            // file_name
            len = strlen(de.file_name);
            current->file_name = malloc(len + 1);
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

SIE_FILE *Sie_FS_FindFilesRecursive(const char *mask) {
    SIE_FILE *top = Sie_FS_FindFiles(mask);
    SIE_FILE *last = Sie_FS_GetLastFile(top);

    SIE_FILE *p = top;
    while (p) {
        if (p->file_attr & FA_DIRECTORY) {
            char *_mask = malloc(strlen(p->dir_name) + strlen(p->file_name) + 2 + 1);
            sprintf(_mask, "%s%s\\*", p->dir_name, p->file_name);
            SIE_FILE *_files = Sie_FS_FindFiles(_mask);
            SIE_FILE *_last = Sie_FS_GetLastFile(_files);
            mfree(_mask);
            if (_files) {
                _files->prev = last;
                last->next = _files;
                last = _last;
            }
        }
        p = p->next;
    }
    return top;
}

void Sie_FS_DestroyFiles(SIE_FILE *top) {
    SIE_FILE *p = top;
    while (p) {
        SIE_FILE *next = p->next;
        mfree(p->dir_name);
        mfree(p->file_name);
        mfree(p);
        p = next;
    }
}

unsigned int Sie_FS_GetFilesCount(SIE_FILE *top) {
    unsigned int count = 0;
    while (top) {
        top = top->next;
        count++;
    }
    return count;
}

SIE_FILE *Sie_FS_GetFileByID(SIE_FILE *top, unsigned int id) {
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

SIE_FILE *Sie_FS_GetFileByFileName(SIE_FILE *top, const char *file_name) {
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

SIE_FILE *Sie_FS_GetLastFile(SIE_FILE *top) {
    SIE_FILE *last = top;
    while (top) {
        if (top->next) {
            last = top->next;
        }
        top = top->next;
    }
    return last;
}

char *Sie_FS_GetPathByFile(SIE_FILE *file) {
    char *path = malloc(strlen(file->dir_name) + strlen(file->file_name) + 1);
    sprintf(path, "%s%s", file->dir_name, file->file_name);
    return path;
}

SIE_FILE *Sie_FS_CopyFileElement(SIE_FILE *file) {
    SIE_FILE *top = malloc(sizeof(SIE_FILE));
    memcpy(top, file, sizeof(SIE_FILE));
    top->prev = NULL;
    top->next = NULL;
    top->dir_name = malloc(strlen(file->dir_name) + 1);
    strcpy(top->dir_name, file->dir_name);
    top->file_name = malloc(strlen(file->file_name) + 1);
    strcpy(top->file_name, file->file_name);
    return top;
}

void Sie_FS_DeleteFilesElement(SIE_FILE *top, SIE_FILE *element) {
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

SIE_FILE *Sie_FS_SortFiles(SIE_FILE *top, int cmp(SIE_FILE*, SIE_FILE*), int keep_folders_on_top) {
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
        Sie_FS_DeleteFilesElement(p, found);
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
                Sie_FS_DeleteFilesElement(p, p);
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
                Sie_FS_DeleteFilesElement(p, p);
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

SIE_FILE *Sie_FS_SortFilesByName(SIE_FILE *top, int keep_folders_on_top) {
    int cmp(SIE_FILE *f1, SIE_FILE *f2) {
        return (strcmpi(f1->file_name, f2->file_name) < 0);
    }
    return Sie_FS_SortFiles(top, cmp, keep_folders_on_top);
}

int Sie_FS_FileExists(const char *path) {
    size_t len = strlen(path);
    WSHDR *ws = AllocWS((ssize_t)len);
    str_2ws(ws, path, len);
    int exists = fexists(ws);
    FreeWS(ws);
    return exists;
}

unsigned int Sie_FS_CopyFile(const char *dest, const char *src) {
    int in = 0, out = 0;
    unsigned int err = 0, err2 = 0;
    unsigned int wb = 0;
    unsigned int result = 0;

    if ((in = _open(src, A_ReadOnly, P_READ, &err)) == -1) {
        goto EXIT;
    }
    if ((out = _open(dest, A_Create + A_WriteOnly + A_Append, P_WRITE, &err)) == -1) {
        goto EXIT;
    }

    FSTATS fstats_in, fstats_out;
    GetFileStats(src, &fstats_in, &err);
    if (err) {
        goto EXIT;
    }
    GetFileStats(dest, &fstats_out, &err);
    if (fstats_out.size || err) {
        goto EXIT;
    }

    char buffer[COPY_BUFFER_SIZE];
    unsigned int offset = 0;
    do {
        _lseek(in, offset, SEEK_SET, &err, &err2);
        if (err || err2) {
            break;
        }
        int rb = _read(in, &buffer, COPY_BUFFER_SIZE, &err);
        if (err) {
            break;
        }
        offset += rb;
        wb += _write(out, buffer, rb, &err);
        if (err) {
            break;
        }
    } while (wb < fstats_in.size);

    EXIT:
        result = 0;
        if (!err) {
            result = wb;
        }
        if (in) {
            _close(in, &err);
        }
        if (out) {
            _close(out, &err);
        }
        return result;
}

int Sie_FS_RemoveDirRecursive(const char *dir) {
    int result = 0;
    char *mask = malloc(strlen(dir) + 1 + 1);
    sprintf(mask, "%s*", dir);
    SIE_FILE *files = Sie_FS_FindFilesRecursive(mask);
    mfree(mask);

    unsigned int err = 0;
    if (files) {
        SIE_FILE *p = NULL;
        SIE_FILE *last = NULL;

        p = files;
        while (1) {
            if (!(p->file_attr & FA_DIRECTORY)) {
                char *path = Sie_FS_GetPathByFile(p);
                if (_unlink(path, &err) != 0) {
                    result = -1;
                }
                mfree(path);
            }
            p = p->next;
            if (p) {
                last = p;
            } else {
                break;
            }
        }
        p = last;
        while (p) {
            if (p->file_attr & FA_DIRECTORY) {
                char *path = Sie_FS_GetPathByFile(p);
                if (_rmdir(path, &err) != 0) {
                    result = -1;
                };
                mfree(path);
            }
            p = p->prev;
        }
        Sie_FS_DestroyFiles(files);
    }
    if (_rmdir(dir, &err) != 0) {
        result = -1;
    }
    return result;
}