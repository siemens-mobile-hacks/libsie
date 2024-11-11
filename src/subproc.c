#include <swilib.h>

#define SUBPROC_NAME        "Sie_SubProc"
#define SUBPROC_START_PG_ID 0x10
#define SUBPROC_MAX_PG      0x0A

unsigned int SLOTS[SUBPROC_MAX_PG];

static void OnMsg(void);

static inline int GetCepID(int pg) {
    return pg * 0x100;
}

void Sie_SubProc_Init() {
    PGROUP pg = {
        0x00,
        #ifdef NEWSGOLD
            "T_SIE",
        #endif
        0x80,
        0x8000,
        1,
    };
    for (int i = 0; i < SUBPROC_MAX_PG; i++) {
        int pg_id = SUBPROC_START_PG_ID + i;
        unsigned short cepid = GetCepID(pg_id);
        if (!GetGBSProcAddress(cepid)) {
            pg.id = pg_id;
            CreateICL(&pg);
            CreateGBSproc(cepid, SUBPROC_NAME, OnMsg, 0x100, 0);
        }
    }
    zeromem(SLOTS, sizeof(unsigned int) * SUBPROC_MAX_PG);
}

void Sie_SubProc_Destroy() {
    for (unsigned int i = 0; i < SUBPROC_MAX_PG; i++) {
        unsigned short cepid = GetCepID(SUBPROC_START_PG_ID + i);
        if (GetGBSProcAddress(cepid)) {
            KillGBSproc(cepid);
        }
    }
}

static void OnMsg() {
    GBS_MSG msg;
    if (GBS_RecActDstMessage(&msg))
    {
        unsigned int slot = msg.msg;
        if (msg.data0) {
            void (*proc)(void *) = msg.data0;
            proc(msg.data1);
            SLOTS[slot] = 0;
        }
    }
}

unsigned int Sie_SubProc_Run(void *proc, void *data) {
    for (unsigned int i = 0; i < SUBPROC_MAX_PG; i++) {
        if (!SLOTS[i]) {
            unsigned short cepid = GetCepID(SUBPROC_START_PG_ID + i);
            SLOTS[i] = 1;
            GBS_SendMessage(cepid, i, 0, proc, data);
            return 1;
        }
    }
    return 0;
}
