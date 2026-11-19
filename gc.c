#include "gc.h"
#include<stdlib.h>
#include<assert.h>

VM* newVM() {
    VM* vm = malloc(sizeof(VM));
    vm->firstObject = NULL;

    vm->stackSize = 0;
    vm->numObjects = 0;
    vm->maxObjects = INITIAL_GC_THRESHOLD;
    return vm;
}

void freeVM(VM* vm) {
    if (vm == NULL) return;

    Object* obj = vm->firstObject;
    while (obj != NULL) {
        Object* next = obj->next;
        free(obj);
        obj = next;
    }
    free(vm);
}

void push(VM* vm, Object* value) {
    assert(vm->stackSize < STACK_MAX && "Stack overflow!");
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
    assert(vm->stackSize > 0 && "Stack underflow!");
    return vm->stack[--vm->stackSize];
}

static void mark(Object* obj) {
    if(obj == NULL || obj->marked) {
        return;
    }

    obj->marked = true;
    if(obj->type == OBJ_PAIR) {
        mark(obj->first);
        mark(obj->second);
    }
}

static void markAll(VM* vm) {
    for(int i = 0; i < vm->stackSize; i++) {
        mark(vm->stack[i]);
    }
}

static void sweep(VM* vm) {
    Object** obj = &vm->firstObject;

    while(*obj) {
        if(!(*obj)->marked) {
            Object* unreached = *obj;

            *obj = unreached->next;
            free(unreached);
            vm->numObjects--;
        } else {
            (*obj)->marked = false;
            obj = &(*obj)->next;
        }
    }
}

void gc(VM* vm) {
    markAll(vm);
    sweep(vm);

    vm->maxObjects = (vm->numObjects > 0 ? vm->numObjects * 2 : INITIAL_GC_THRESHOLD);
}

Object* newObject(VM* vm, ObjectType type) {
    if(vm->numObjects >= vm->maxObjects) {
        gc(vm);
    }

    Object* object = malloc(sizeof(Object));
    object->type = type;
    object->marked = false;

    object->next = vm->firstObject;
    vm->firstObject = object;
    vm->numObjects++;

    return object;
}

void pushInt(VM* vm, int intValue) {
    Object* object = newObject(vm, OBJ_INT);
    object->value = intValue;
    push(vm, object);
}

Object* pushPair(VM* vm) {
    assert(vm->stackSize >= 2 && "Pair requires two stack values!");

    Object* object = newObject(vm, OBJ_PAIR);
    object->first = pop(vm);
    object->second = pop(vm);

    push(vm, object);
    return object;
}