#include <swilib.h>
#include <stdlib.h>

CSM_RAM *FindCSMByConstr(CSM_RAM *csm, void *constr) {
    while (csm) {
        if (csm->constr == constr) {
            return csm;
        }
        csm = csm->next;
    }
    return NULL;
}

CSM_RAM *Sie_CSM_FindByAddr(const char *addr) {
    void *constr = (void*)strtoul(addr, NULL, 16);
    CSM_RAM *csm = FindCSMByConstr(CSM_root()->csm_q->csm.first, constr);
    if (!csm) {
        csm = FindCSMByConstr(CSM_root()->csm_q->csm_background.first, constr);
    }
    return csm;
}
