#define STACK_MAX 256

typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;

    union {
        /* OBJ_INT */
        int value;

        /* OBJ_PAIR */
        struct {
            struct sObject* first;
            struct sObject* second  ;
        };
    };
} Object;

typedef struct {
  Object* stack[STACK_MAX];
  int stackSize;
} VM;