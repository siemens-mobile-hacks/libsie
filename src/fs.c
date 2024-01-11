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
        if (p->file_attr & SIE_FS_FA_DIRECTORY) {
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

SIE_FILE *Sie_FS_CloneFiles(SIE_FILE *top) {
    if (top) {
        SIE_FILE *files = Sie_FS_CopyFileElement(top);
        SIE_FILE *current = files;
        top = top->next;
        while (top) {
            SIE_FILE *file = Sie_FS_CopyFileElement(top);
            file->prev = current;
            current->next = file;
            current = file;
            top = top->next;
        }
        return files;
    }
    return NULL;
}

void Sie_FS_DestroyFiles(SIE_FILE *top) {
    SIE_FILE *p = top;
    while (p) {
        SIE_FILE *next = p->next;
        Sie_FS_DestroyFileElement(p);
        p = next;
    }
}

/**********************************************************************************************************************/

unsigned int Sie_FS_ContainsFile(SIE_FILE *top, SIE_FILE *file) {
    while (top) {
        if (strcmp(top->dir_name, file->dir_name) == 0 &&
            strcmp(top->file_name, file->file_name) == 0) {
            return 1;
        }
        top = top->next;
    }
    return 0;
}

unsigned int Sie_FS_GetFilesCount(SIE_FILE *top) {
    unsigned int count = 0;
    while (top) {
        top = top->next;
        count++;
    }
    return count;
}

SIE_FILE *Sie_FS_GetFirstFile(SIE_FILE *top) {
    SIE_FILE *first = top;
    while (top) {
        if (top->prev) {
            first = top->prev;
        }
        top = top->prev;
    }
    return first;
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

SIE_FILE *Sie_FS_GetFileByID(SIE_FILE *top, unsigned int id) {
    unsigned int i = 0;
    while (top) {
        if (i == id) {
            return top;
        }
        top = top->next;
        i++;
    }
    return NULL;
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

SIE_FILE *Sie_FS_GetFileByAlias(SIE_FILE *top, const char *alias) {
    SIE_FILE *file = NULL;
    while (top) {
        if (top->alias && strcmpi(top->alias, alias) == 0) {
            file = top;
            break;
        }
        top = top->next;
    }
    return file;
}

SIE_FILE *Sie_FS_GetUniqueFile(SIE_FILE *file) {
    const size_t len_suffix = 32;
    char *file_name = malloc(strlen(file->file_name) + len_suffix + 1);
    char *dest_path = malloc(strlen(file->dir_name) + strlen(file->file_name) + len_suffix + 1);

    sprintf(dest_path, "%s%s", file->dir_name, file->file_name);
    if (!Sie_FS_FileExists(dest_path)) {
        mfree(file_name);
        mfree(dest_path);
        return Sie_FS_CopyFileElement(file);
    }
    unsigned int i = 0;
    while (1) {
        char suffix[len_suffix], *ext;
        sprintf(suffix, "(%d)", i + 1);
        ext = strrchr(file->file_name, '.');
        if (ext) {
            strncpy(file_name, file->file_name, ext - file->file_name);
            file_name[ext - file->file_name] = '\0';
            strcat(file_name, suffix);
            strcat(file_name, ext);
        } else {
            strcpy(file_name, file->file_name);
            strcat(file_name, suffix);
        }
        sprintf(dest_path, "%s%s", file->dir_name, file_name);
        if (!Sie_FS_FileExists(dest_path)) {
            SIE_FILE *unique = Sie_FS_CopyFileElement(file);
            unique->file_name = file_name;
            mfree(dest_path);
            return unique;
        } else {
            i += 1;
        }
    }
}

char *Sie_FS_GetPathByFile(SIE_FILE *file) {
    char *path = malloc(strlen(file->dir_name) + strlen(file->file_name) + 1);
    sprintf(path, "%s%s", file->dir_name, file->file_name);
    return path;
}

SIE_FILE *Sie_FS_CopyFileElement(SIE_FILE *file) {
    SIE_FILE *new = malloc(sizeof(SIE_FILE));
    memcpy(new, file, sizeof(SIE_FILE));
    new->prev = NULL;
    new->next = NULL;
    new->dir_name = malloc(strlen(file->dir_name) + 1);
    strcpy(new->dir_name, file->dir_name);
    new->file_name = malloc(strlen(file->file_name) + 1);
    strcpy(new->file_name, file->file_name);
    if (file->alias) {
        new->alias = malloc(strlen(file->alias) + 1);
        strcpy(new->alias, file->alias);
    }
    return new;
}

void Sie_FS_DestroyFileElement(SIE_FILE *file) {
    mfree(file->dir_name);
    mfree(file->file_name);
    if (file->alias) {
        mfree(file->alias);
    }
    mfree(file);
}

SIE_FILE *Sie_FS_DeleteFileElement(SIE_FILE *top, SIE_FILE *file) {
#define _cmp (strcmpi(current->dir_name, file->dir_name) == 0 && strcmp(current->file_name, file->file_name) == 0)
    while (top) {
        SIE_FILE *current = top;
        if (_cmp) {
            SIE_FILE *prev = current->prev;
            SIE_FILE *next = current->next;
            if (prev) {
                prev->next = next;
            }
            if (next) {
                next->prev = prev;
            }
            return current;
        }
        top = top->next;
    }
    return NULL;
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
        current = Sie_FS_CopyFileElement(found);
        if (found == p) {
            p = p->next;
        }
        SIE_FILE *element = Sie_FS_DeleteFileElement(p, found);
        if (element) {
            Sie_FS_DestroyFileElement(element);
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
            SIE_FILE *element = NULL;
            if (p->file_attr & SIE_FS_FA_DIRECTORY) {
                SIE_FILE *current_dir = Sie_FS_CopyFileElement(p);
                element = Sie_FS_DeleteFileElement(p, p);
                current_dir->next = NULL;
                current_dir->prev = last_dir;
                if (!last_dir) {
                    top_dir = current_dir;
                } else {
                    last_dir->next = current_dir;
                }
                last_dir = current_dir;
            } else {
                SIE_FILE *current_file = Sie_FS_CopyFileElement(p);
                element = Sie_FS_DeleteFileElement(p, p);
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
            if (element) {
                Sie_FS_DestroyFileElement(element);
            }
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

static int cmp_by_name(SIE_FILE *f1, SIE_FILE *f2) {
    return (strcmpi(f1->file_name, f2->file_name) < 0);
}

SIE_FILE *Sie_FS_SortFilesByName(SIE_FILE *top, int keep_folders_on_top) {
    return Sie_FS_SortFiles(top, cmp_by_name, keep_folders_on_top);
}

int Sie_FS_FileExists(const char *path) {
    size_t len = strlen(path);
    WSHDR *ws = AllocWS((int)len);
    str_2ws(ws, path, len);
    int exists = fexists(ws);
    FreeWS(ws);
    return exists;
}

int Sie_FS_MMCardExists() {
    unsigned int err;
    return GetTotalFlexSpace(4, &err) ? 1 : 0;
}

int Sie_FS_CreateFile(const char *path, unsigned int *err) {
    int fp;
    fp = _open(path, A_Create + A_WriteOnly, P_WRITE, err);
    if (fp != -1) {
        _close(fp, err);
        return 1;
    } else {
        return 0;
    }
}

unsigned int Sie_FS_CopyFile(const char *src, const char *dest, unsigned int *err) {
    int in = -1, out = -1;
    unsigned int err1 = 0, err2 = 0;
    unsigned int wb = 0;
    unsigned int result = 0;

    in = _open(src, A_ReadOnly, P_READ, &err1);
    if (in == -1) {
        goto EXIT;
    }
    out = _open(dest, A_Create + A_WriteOnly + A_Append, P_WRITE, &err1);
    if (out == -1) {
        goto EXIT;
    }

    FSTATS fstats_in, fstats_out;
    GetFileStats(src, &fstats_in, &err1);
    if (err1) {
        goto EXIT;
    }
    GetFileStats(dest, &fstats_out, &err1);
    if (fstats_out.size || err1) {
        goto EXIT;
    }

    char buffer[COPY_BUFFER_SIZE];
    unsigned int offset = 0;
    do {
        _lseek(in, offset, SEEK_SET, &err1, &err2);
        if (err1 || err2) {
            break;
        }
        int rb = _read(in, &buffer, COPY_BUFFER_SIZE, &err1);
        if (err1) {
            break;
        }
        offset += rb;
        wb += _write(out, buffer, rb, &err1);
        if (err1) {
            break;
        }
    } while (wb < fstats_in.size);
    EXIT:
        result = 0;
        if (err1) {
            *err = err1;
        } else {
            result = wb;
        }
        if (in != -1) {
            _close(in, &err1);
        }
        if (out != -1) {
            _close(out, &err1);
        }
        return result;
}

int Sie_FS_DeleteFilesRecursive(const char *dir) {
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
            if (!(p->file_attr & SIE_FS_FA_DIRECTORY)) {
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
            if (p->file_attr & SIE_FS_FA_DIRECTORY) {
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
