#include <swilib.h>

__attribute_constructor
void InitLibrary() {
    extern void InitSubProc();
    InitSubProc();
}

__attribute_destructor
void DestroyLibrary() {
    extern void DestroySubProc();
    DestroySubProc();
}
