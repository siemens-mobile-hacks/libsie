#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../include/sie/fs/fs.h"

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

int Sie_FS_GetIDByFileName(SIE_FILE *top, const char *file_name) {
    int i = 0;
    while (top) {
        if (strcmpi(top->file_name, file_name) == 0) {
            return i;
        }
        top = top->next;
        i++;
    }
    return -1;
}

char *Sie_FS_GetPathByFile(SIE_FILE *file) {
    char *path = malloc(strlen(file->dir_name) + strlen(file->file_name) + 1);
    sprintf(path, "%s%s", file->dir_name, file->file_name);
    return path;
}

/**********************************************************************************************************************/

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

SIE_FILE *Sie_FS_ExcludeFilesByFileAttr(SIE_FILE *top, short file_attr) {
    SIE_FILE *file = top;
    while (file) {
        SIE_FILE *prev = file->prev;
        SIE_FILE *next = file->next;
        if (file->file_attr & file_attr) {
            if (prev) {
                prev->next = next;
            }
            if (next) {
                next->prev = prev;
            }
            if (file == top) {
                top = next;
            }
        }
        file = next;
    }
    return top;
}

unsigned int Sie_FS_FileExists(const char *path) {
    size_t len = strlen(path);
    WSHDR *ws = AllocWS((int)len + 5);
    str_2ws(ws, path, len);
    unsigned int exists = fexists(ws);
    FreeWS(ws);
    if (!exists) {
        unsigned int err = 0;
        exists = Sie_FS_IsDir(path, &err);
    }
    return exists;
}

unsigned int Sie_FS_MMCardExists() {
    unsigned int err;
    return GetTotalFlexSpace(4, &err) ? 1 : 0;
}

unsigned int Sie_FS_IsDir(const char *path, unsigned int *err) {
    return isdir(path, err);
}

unsigned int Sie_FS_CreateFile(const char *path, unsigned int *err) {
    int fp;
    fp = _open(path, A_Create + A_WriteOnly, P_WRITE, err);
    if (fp != -1) {
        _close(fp, err);
        return 1;
    } else {
        return 0;
    }
}

unsigned int Sie_FS_CreateDir(const char *path, unsigned int *err) {
    return _mkdir(path, err);
}

unsigned int Sie_FS_CreateDirs(const char *path, unsigned int *err) {
    char *p1 = (char*)path + 3;
    while (1) {
        p1 = strchr(p1, '\\');
        if (p1) {
            unsigned int _err;
            size_t len = p1 - path;
            char *dir = malloc(len + 1);
            strncpy(dir, path, len);
            dir[len] = '\0';
            if (!Sie_FS_FileExists(dir)) {
                if (!Sie_FS_CreateDir(dir, &_err)) {
                    *err = _err;
                    return 0;
                }
            }
            p1++;
        }
        else break;
    }
    return 1;
}

int Sie_FS_CopyFile(const char *src, const char *dest, unsigned int *err) {
    int result = -1;
    int in = -1, out = -1;
    unsigned int err1 = 0, err2 = 0;
    unsigned int wb = 0;

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
        if (err1) {
            *err = err1;
        } else {
            result = (int)wb;
        }
        if (in != -1) {
            _close(in, &err1);
        }
        if (out != -1) {
            _close(out, &err1);
        }
        return result;
}

unsigned int Sie_FS_CopyDir(const char *src, const char *dest, unsigned int *err, int (*get_cancel_flag)()) {
    char *mask = malloc(strlen(src) + 2 + 1);
    sprintf(mask, "%s\\*", src);
    SIE_FILE *files = Sie_FS_FindFilesRecursive(mask);
    mfree(mask);

    unsigned int result = 0;
    if (Sie_FS_CreateDir(dest, err)) {
        result = 1;
        SIE_FILE *file = files;
        while (file) {
            if (get_cancel_flag && get_cancel_flag()) {
                break;
            }
            char *relative = file->dir_name + strlen(src);
            size_t len_relative = strlen(relative);
            size_t len_file_name = strlen(file->file_name);

            char *s = malloc(strlen(src) + len_relative + len_file_name + 1);
            char *d = malloc(strlen(dest) + len_relative + len_file_name + 1);
            sprintf(s, "%s%s%s", src, relative, file->file_name);
            sprintf(d, "%s%s%s", dest, relative, file->file_name);
            if (file->file_attr & SIE_FS_FA_DIRECTORY) {
                if (!Sie_FS_CreateDir(d, err)) {
                    result = 0;
                }
            } else {
                if (Sie_FS_CopyFile(s, d, err) < 0) {
                    result = 0;
                }
            }
            mfree(s);
            mfree(d);
            if (!result) {
                break;
            }
            file = file->next;
        }
        Sie_FS_DestroyFiles(files);
    }
    return result;
}

unsigned int Sie_FS_MoveFile(const char *src, const char *dest, unsigned int *err, int (*get_cancel_flag)()) {
    if (src[0] == dest[0]) { // check disk number
        return fmove(src, dest, err);
    } else {
        unsigned int result = 0, err1 = 0;
        if (!Sie_FS_IsDir(src, &err1)) {
            if (Sie_FS_CopyFile(src, dest, &err1) >= 0) {
                result = Sie_FS_DeleteFile(src, &err1);
            }
            *err = err1;
        } else {
            if (Sie_FS_CopyDir(src, dest, &err1, get_cancel_flag)) {
                result = Sie_FS_DeleteDirRecursive(src, &err1);
            }
            *err = err1;
        }
        return result;
    }
}

unsigned int Sie_FS_DeleteFile(const char *path, unsigned int *err) {
    return _unlink(path, err);
}

unsigned int Sie_FS_DeleteDir(const char *path, unsigned int *err) {
    return _rmdir(path, err);
}

unsigned Sie_FS_DeleteDirRecursive(const char *path, unsigned int *err) {
    int result = 1;
    size_t len = strlen(path);
    char *mask = malloc(len + 2 + 1);
    if (path[len - 1] == '\\') {
        sprintf(mask, "%s*", path);
    } else {
        sprintf(mask, "%s\\*", path);
    }
    SIE_FILE *files = Sie_FS_FindFilesRecursive(mask);
    mfree(mask);

    if (files) {
        SIE_FILE *file = NULL;
        SIE_FILE *last = NULL;

        file = files;
        while (1) {
            if (!(file->file_attr & SIE_FS_FA_DIRECTORY)) {
                char *p = Sie_FS_GetPathByFile(file);
                if (!Sie_FS_DeleteFile(p, err)) {
                    result = 0;
                }
                mfree(p);
            }
            if (!result) {
                break;
            }
            file = file->next;
            if (file) {
                last = file;
            } else {
                break;
            }
        }
        file = last;
        while (file) {
            if (file->file_attr & SIE_FS_FA_DIRECTORY) {
                char *p = Sie_FS_GetPathByFile(file);
                if (!Sie_FS_DeleteDir(p, err)) {
                    result = 0;
                }
                mfree(p);
            }
            if (!result) {
                break;
            }
            file = file->prev;
        }
        Sie_FS_DestroyFiles(files);
    }
    return (result) ? Sie_FS_DeleteDir(path, err) : result;
}
