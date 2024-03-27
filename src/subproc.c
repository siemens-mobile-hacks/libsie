#include <swilib.h>

#define SUBPROC_NAME        "Sie_SubProc"
#define SUBPROC_START_PG_ID 0x10
#define SUBPROC_MAX_PG      0x0A

unsigned int SLOTS[SUBPROC_MAX_PG];

static void OnMsg(void);

static inline short GetCepID(int pg) {
    return pg * 0x100;
}

void Sie_SubProc_Init() {
    PGROUP pg = {
            0x00,
            "T_SIE",
            0x80,
            0x8000,
            2,
    };
    int pg_id;
    short cepid;
    for (int i = 0; i < SUBPROC_MAX_PG; i++) {
        pg_id = SUBPROC_START_PG_ID + i;
        cepid = GetCepID(pg_id);
        if (!GetGBSProcAddress(cepid)) {
            pg.id = pg_id;
            CreateICL(&pg);
            CreateGBSproc(cepid, SUBPROC_NAME, OnMsg, 0x100, 0);
        }
    }
    for (unsigned int i = 0; i < SUBPROC_MAX_PG; i++) {
        pg_id = SUBPROC_START_PG_ID + i;
        cepid = GetCepID(pg_id) + 1;
        if (!GetGBSProcAddress(cepid)) {
            CreateGBSproc(cepid, SUBPROC_NAME, OnMsg,
                          0x80, 0);
        }
    }
    zeromem(SLOTS, sizeof(unsigned int) * 10);
}

void Sie_SubProc_Destroy() {
    for (unsigned int i = 0; i <= SUBPROC_MAX_PG; i++) {
        KillGBSproc(GetCepID(SUBPROC_START_PG_ID + i) + 1);
    }
}

static void OnMsg() {
    GBS_MSG msg;
    if (GBS_RecActDstMessage(&msg))
    {
        unsigned int slot = msg.msg;
        if (msg.data0)
        {
            void (*proc)(void *) = (void (*)(void *))msg.data0;
            proc(msg.data1);
            SLOTS[slot] = 0;
        }
    }
}

unsigned int Sie_SubProc_Run(void *proc, void *data) {
    for (unsigned int i = 0; i < SUBPROC_MAX_PG; i++) {
        if (!SLOTS[i]) {
            SLOTS[i] = 1;
            GBS_SendMessage(GetCepID(SUBPROC_START_PG_ID + i) + 1, i, 0, proc, data);
            return 1;
        }
    }
    return 0;
}
