#include <swilib.h>
#include <stdlib.h>

CSM_RAM *FindCSMByConstr(CSM_RAM *csm, void *constr) {
    CSM_RAM *csm_ram = NULL;
    LockSched();
    while (csm) {
        if (csm->constr == constr) {
            csm_ram = csm;
            break;
        }
        csm = csm->next;
    }
    UnlockSched();
    return csm_ram;
}

CSM_RAM *Sie_CSM_FindByAddr(const char *addr) {
    void *constr = (void*)strtoul(addr, NULL, 16);
    CSM_RAM *csm = FindCSMByConstr(CSM_root()->csm_q->csm.first, constr);
    #ifdef NEWSGOLD
        if (!csm) {
            csm = FindCSMByConstr(CSM_root()->csm_q->csm_background.first, constr);
        }
    #endif
    return csm;
}
