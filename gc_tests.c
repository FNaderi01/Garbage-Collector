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
    assert(vm->stackSize == 1);
    assert(vm->numObjects == 3);
    assert(obj->first->value == 2);
    assert(obj->second->value == 1);

    freeVM(vm);
}


static void testUnreachableObjects(void) {
    VM* vm = newVM();

    pushInt(vm, 1);

    assert(vm->numObjects == 1);
    assert(vm->stackSize == 1);

    assert(pop(vm)->value == 1);
    assert(vm->stackSize == 0);
    assert(vm->numObjects == 1);

    gc(vm);

    assert(vm->numObjects == 0);
    assert(vm->stackSize == 0);
    assert(vm->firstObject == NULL);

    freeVM(vm);
}

void testNestedPairs(void) {
    VM* vm = newVM();

    pushInt(vm, 1);
    pushInt(vm, 2);
    Object* inner = pushPair(vm);
    assert(inner->first->value == 2);
    assert(inner->second->value == 1);

    pushInt(vm, 3);
    Object* outer = pushPair(vm);
    assert(outer->first->value == 3);
    assert(outer->second == inner);
    assert(outer->second->first->value == 2);
    assert(outer->second->second->value == 1);

    gc(vm);
    assert(vm->stackSize == 1);
    assert(vm->numObjects == 5);

    pop(vm);
    gc(vm);
    assert(vm->numObjects == 0);
    assert(vm->stackSize == 0);

    freeVM(vm);
}

static void testSweepHeadMiddleTail(void) {
    VM* vm = newVM();
    Object* tail = newObject(vm, OBJ_INT);
    Object* keep1 = newObject(vm, OBJ_INT);
    Object* middle = newObject(vm, OBJ_INT);
    Object* keep2 = newObject(vm, OBJ_INT);
    Object* head = newObject(vm, OBJ_INT);
    (void)tail;
    (void)middle;
    (void)head;

    push(vm, keep1);
    push(vm, keep2);
    gc(vm);

    assert(vm->numObjects == 2);
    assert(vm->firstObject == keep2);
    assert(keep2->next == keep1);
    assert(keep1->next == NULL);
    assert(keep1->marked == 0 && keep2->marked == 0);
    freeVM(vm);
}

static void testSharedReferences(void) {
    VM* vm = newVM();
    pushInt(vm, 7);
    Object* shared = pop(vm);
    push(vm, shared);
    push(vm, shared);
    Object* pair = pushPair(vm);

    gc(vm);
    assert(vm->numObjects == 2);
    assert(pair->first == shared);
    assert(pair->second == shared);
    assert(shared->value == 7);
    freeVM(vm);
}

static void testCycle(void) {
    VM* vm = newVM();
    Object* pair = newObject(vm, OBJ_PAIR);
    pair->first = pair;
    pair->second = NULL;
    push(vm, pair);

    gc(vm);
    assert(vm->numObjects == 1);
    assert(pair->first == pair);
    assert(pair->marked == 0);

    pop(vm);
    gc(vm);
    assert(vm->numObjects == 0);
    freeVM(vm);
}

static void testNullChildren(void) {
    VM* vm = newVM();
    Object* pair = newObject(vm, OBJ_PAIR);
    pair->first = NULL;
    pair->second = NULL;
    push(vm, pair);
    gc(vm);
    assert(vm->numObjects == 1);
    freeVM(vm);
}

static void testAutomaticGC(void) {
    VM* vm = newVM();
    vm->maxObjects = 2;
    pushInt(vm, 1);
    pushInt(vm, 2);
    pop(vm);
    pop(vm);

    pushInt(vm, 3); /* newObject triggers GC before allocation */
    assert(vm->numObjects == 1);
    assert(vm->firstObject == vm->stack[0]);
    assert(vm->stack[0]->value == 3);
    assert(vm->maxObjects == INITIAL_GC_THRESHOLD);
    freeVM(vm);
}

static void testThresholdGrowth(void) {
    VM* vm = newVM();
    vm->maxObjects = 2;
    pushInt(vm, 10);
    pushInt(vm, 20);
    pushInt(vm, 30); /* 2 survivors -> threshold grows to 4 */

    assert(vm->numObjects == 3);
    assert(vm->maxObjects == 4);
    assert(vm->stackSize == 3);
    freeVM(vm);
}

#define RUN_TEST(fn) do { fn(); puts("PASS: " #fn); } while (0)


int main() {
    RUN_TEST(testEmptyGC);
    RUN_TEST(testPushIntObject);
    RUN_TEST(testPushPairObject);
    RUN_TEST(testPushInt);
    RUN_TEST(testPushPair);
    RUN_TEST(testUnreachableObjects);
    RUN_TEST(testNestedPairs);
    RUN_TEST(testSweepHeadMiddleTail);
    RUN_TEST(testSharedReferences);
    RUN_TEST(testCycle);
    RUN_TEST(testNullChildren);
    RUN_TEST(testAutomaticGC);
    RUN_TEST(testThresholdGrowth);
   
    puts("All 13 tests passed!");
    return 0;
}