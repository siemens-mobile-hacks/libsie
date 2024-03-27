#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include <cfg_items.h>

static size_t GetSize(CFG_HDR *cfghdr0, const int *config_begin, const int *config_end) {
    volatile unsigned int segb = (volatile unsigned int)config_begin;
    volatile unsigned int sege = (volatile unsigned int)config_end;
    return (sege - segb) - 4;
}

int Sie_Config_Save(const char *path, CFG_HDR *cfghdr0, const int *config_begin, const int *config_end) {
    unsigned int err;
    int fp = _open(path, A_ReadWrite + A_Create + A_Truncate,
                   P_READ + P_WRITE, &err);
    if (fp != -1) {
        size_t size = GetSize(cfghdr0, config_begin, config_end);
        int write_size = _write(fp, cfghdr0, (int)size, &err);
        _close(fp, &err);
        if (size == write_size) {
            return 1;
        }
    }
    return 0;
}

int Sie_Config_Load(const char *path, CFG_HDR *cfghdr0, const int *config_begin, const int *config_end) {
    int result = 0;
    size_t size = GetSize(cfghdr0, config_begin, config_end);

    char *buf = malloc(size);
    if (buf) {
        unsigned int err;
        int fp = _open(path, A_ReadOnly + A_BIN, 0, &err);
        if (fp != -1) {
            if (_read(fp, buf, (int)size, &err) == size) {
                memcpy(cfghdr0, buf, size);
                _close(fp, &err);
                result = 2;
            } else {
                _close(fp, &err);
                goto SAVE_NEW_CFG;
            }
        } else {
            SAVE_NEW_CFG:
            result = Sie_Config_Save(path, cfghdr0, config_begin, config_end);
        }
        mfree(buf);
    }
    return result;
}
