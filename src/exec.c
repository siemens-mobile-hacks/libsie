#include <swilib.h>
#include <stdlib.h>

typedef void (*ExecProc)();

int Sie_Exec_File(const char *path) {
    int result = 0;
    size_t len = strlen(path);
    WSHDR *ws = AllocWS((int)len);
    str_2ws(ws, path, len);
    result = ExecuteFile(ws, 0, NULL);
    FreeWS(ws);
    return result;
}

int Sie_Exec_Shortcut(const char *shortcut) {
    unsigned int *addr = (unsigned int*)GetFunctionPointer((char*)shortcut);
    if (addr) {
#ifdef NEWSGOLD
        ExecProc p = (ExecProc)*(addr + 4);
#else
        ExecProc p = (ExecProc)addr;
#endif
        SUBPROC(p);
        return 1;
    }
    return 0;
}

int Sie_Exec_Entrypoint(const char *entrypoint) {
    unsigned int addr = strtoul(entrypoint, 0, 16);
    if (addr) {
        ExecProc p = (ExecProc)addr;
        SUBPROC(p);
        return 1;
    }
    return 0;
}

int Sie_Exec_Execute(const char *param) {
    if (param) {
        size_t len = strlen(param);
        if (len > 3) {
            if (param[0] >= '0' && param[0] <= '4' && param[1] == ':' && param[2] == '\\') { // file
                return Sie_Exec_File(param);
            } else if (len == 8 && (param[0] == 'A' || param[0] == 'a')) { // entrypoint
                return Sie_Exec_Entrypoint(param);
            } else { //shortcut
                return Sie_Exec_Shortcut(param);
            }
        }
    }
    return 0;
}
