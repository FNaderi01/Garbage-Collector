#include "gc.h"
#include <assert.h>
#include <stdio.h>

static void testEmptyGC(void) {
    VM* vm = newVM();

    gc(vm);

    assert(vm->numObjects == 0);
    assert(vm->firstObject == NULL);
    assert(vm->maxObjects == INITIAL_GC_THRESHOLD);

    freeVM(vm);
}

static void testPushIntObject(void) {
    VM* vm = newVM();
    Object* obj = newObject(vm, OBJ_INT);
    obj->value = 1;

    push(vm, obj);
    assert(vm->stackSize == 1);
    assert(vm->numObjects == 1);
    
    assert(pop(vm) == obj);
    assert(vm->stackSize == 0);
    assert(vm->numObjects == 1);

    freeVM(vm);
}

static void testPushPairObject(void) {
    VM* vm = newVM();

    Object* f = newObject(vm, OBJ_INT);
    Object* s = newObject(vm, OBJ_INT);
    Object* obj = newObject(vm, OBJ_PAIR);

    obj->first = f;
    obj->second = s;

    push(vm, obj);
    assert(vm->stackSize == 1);
    assert(vm->numObjects == 3);

    assert(pop(vm) == obj);
    assert(vm->stackSize == 0);
    assert(vm->numObjects == 3);

    freeVM(vm);
}

static void testPushInt(void) {
    VM* vm = newVM();

    pushInt(vm, 1);

    Object* obj = pop(vm);
    assert(obj->value == 1);

    freeVM(vm);
}

static void testPushPair(void) {
    VM* vm = newVM();

    pushInt(vm, 1);
    pushInt(vm, 2);

    Object* obj = pushPair(vm);
    assert(obj->type == OBJ_PAIR);
    assert(obj->first->value == 2);
    assert(obj->second->value == 1);

    freeVM(vm);
}


static void testUnreachableObjects(void) {
    VM* vm = newVM();

    Object* obj = newObject(vm, OBJ_INT);
    push(vm, obj);

    assert(vm->numObjects == 1);
    assert(vm->stackSize == 1);

    pop(vm);
    assert(vm->stackSize == 0);
    assert(vm->numObjects == 1);

    gc(vm);

    assert(vm->numObjects == 0);
    assert(vm->firstObject == NULL);

    freeVM(vm);
}

int main() {
    testEmptyGC();
    testPushIntObject();
    testPushPairObject();
    testPushInt();
    testPushPair();
    testUnreachableObjects();
}