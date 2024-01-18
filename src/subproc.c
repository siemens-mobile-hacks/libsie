#include <swilib.h>

#define SUBPROC_NAME "Sie_SubProc"
#define SUBPROC_MAX_SLOTS 2

int SLOTS[SUBPROC_MAX_SLOTS];
int CEPIDS[SUBPROC_MAX_SLOTS] = {0x4409, 0x4311};
int PRIOS[SUBPROC_MAX_SLOTS] = {0x80, 0x80};

static void OnMsg(void);

void Sie_SubProc_Init() {
    for (unsigned int i = 0; i < SUBPROC_MAX_SLOTS; i++) {
        SLOTS[i] = 0;
        CreateGBSproc(CEPIDS[i], SUBPROC_NAME, OnMsg, PRIOS[i], 0);
    }
}

void Sie_SubProc_Destroy() {
    for (unsigned int i = 0; i < SUBPROC_MAX_SLOTS; i++) {
        KillGBSproc(CEPIDS[i]);
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
    int slot = 0;
    while (slot < SUBPROC_MAX_SLOTS) {
        if (!SLOTS[slot]) {
            SLOTS[slot] = 1;
            GBS_SendMessage(CEPIDS[slot], slot, 0, proc, data);
            return 1;
        } else {
            slot++;
        }
    }
    return 0;
}
