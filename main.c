#define STACK_MAX 256

typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;
    unsigned char marked;
    struct sObject* next;

    union {
        /* OBJ_INT */
        int value;

        /* OBJ_PAIR */
        struct {
            struct sObject* first;
            struct sObject* second;
        };
    };
} Object;

typedef struct {
  Object* stack[STACK_MAX];
  int stackSize;
  Object* firstObject;
} VM;

VM* newVM() {
    VM* vm = malloc(sizeof(VM));
    vm->stackSize = 0;
    vm->firstObject = nullptr;
    return vm;
}

void push(VM* vm, Object* value) {
    assert(vm->stackSize < STACK_MAX, "Stack overflow!");
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
    assert(vm->stackSize > 0, "Stack underflow!");
    return vm->stack[--vm->stackSize];
}

Object* newObject(VM* vm, ObjectType type) {
    Object* object = malloc(sizeof(Object));
    object->type = type;
    object->marked = false;

    object->next = vm->firstObject;
    vm->firstObject = object;

    return object;
}

void pushInt(VM* vm, int intValue) {
    Object* object = newObject(vm, OBJ_INT);
    object->value = intValue;
    push(vm, object);
}

Object* pushPair(VM* vm) {
    Object* object = newObject(vm, OBJ_PAIR);
    object->first = pop(vm);
    object->second = pop(vm);

    push(vm, object);
    return object;
}

void markAll(VM* vm) {
    for(int i = 0; i < vm->stackSize; i++) {
        mark(vm->stack[i]);
    }
}

void mark(Object* obj) {
    if(obj->marked) {
        return;
    }

    obj->marked = true;
    if(obj->type == OBJ_PAIR) {
        mark(obj->first);
        mark(obj->second);
    }
}

void sweep(VM* vm) {
    Object** obj = &vm->firstObject;

    while(*obj) {
        if(!(*obj)->marked) {
            Object* unreached = *obj;

            *obj = unreached->next;
            free(unreached);
        } else {
            (*obj)->marked = false;
            obj = &(*obj)->next;
        }
    }
}