#include <swilib.h>

#define SUBPROC_NAME        "Sie_SubProc"
#define SUBPROC_PG_ID       0x50
#define SUBPROC_MAX_MEMBERS 0x0A
#define SUBPROC_START_CEPID (SUBPROC_PG_ID * 0x100)

unsigned int SLOTS[SUBPROC_MAX_MEMBERS];

static void OnMsg(void);

void Sie_SubProc_Init() {
    PGROUP pg = {
            SUBPROC_PG_ID,
            "T_SIE",
            0x80,
            0x8000,
            SUBPROC_MAX_MEMBERS + 1,
    };
    if (!GetGBSProcAddress(SUBPROC_START_CEPID)) {
        CreateICL(&pg);
        CreateGBSproc(SUBPROC_START_CEPID, SUBPROC_NAME, OnMsg, 0x100, 0);
    }
    zeromem(SLOTS, sizeof(unsigned int) * 10);
    for (unsigned int i = 1; i <= SUBPROC_MAX_MEMBERS; i++) {
        CreateGBSproc(SUBPROC_START_CEPID + i, SUBPROC_NAME, OnMsg, 0x80, 0);
    }
}

void Sie_SubProc_Destroy() {
    for (unsigned int i = 1; i <= SUBPROC_MAX_MEMBERS; i++) {
        KillGBSproc(SUBPROC_START_CEPID + i);
    }
}

static void OnMsg()
{
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

unsigned int Sie_SubProc_Run(void *proc, void *data)
{
    for (unsigned int i = 0; i < SUBPROC_MAX_MEMBERS; i++) {
        if (!SLOTS[i]) {
            SLOTS[i] = 1;
            GBS_SendMessage(SUBPROC_START_CEPID + 1 + i, i, 0, proc, data);
            return 1;
        }
    }
    return 0;
}
