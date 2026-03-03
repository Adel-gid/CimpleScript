#include "object.h"

typedef struct ArrayData {
    CSObject* object;
    struct ArrayData* next;
} ArrayData;

CSObject* arrayAppend(CSObject* self, CSObject* another) {
    return NULL;
}

CSObject* arrayGetAttr(CSObject* self, CSObject* name) {

}

