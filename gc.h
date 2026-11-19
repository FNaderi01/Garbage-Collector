#ifndef GC_H
#define GC_H

#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 100

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
  Object* firstObject;

  int stackSize;
  int numObjects;
  int maxObjects;
} VM;

VM* newVM();
void freeVM(VM* vm);

void push(VM* vm, Object* obj);
Object* pop(VM* vm);

Object* newObject(VM* vm, ObjectType type);
void pushInt(VM* vm, int value);
Object* pushPair(VM* vm);

void gc(VM* vm);

#endif