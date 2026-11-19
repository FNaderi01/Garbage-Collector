#include "gc.h"
#include <assert.h>
#include <stdio.h>

void testEmptyGC(void) {
    VM* vm = newVM();

    gc(vm);

    assert(vm->numObjects == 0);
    assert(vm->firstObject == NULL);
    assert(vm->maxObjects == INITIAL_GC_THRESHOLD);

    freeVM(vm);
}

int main() {
    testEmptyGC();
}