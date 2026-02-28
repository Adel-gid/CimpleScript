#include "object.h"
#include <malloc.h>
#include <string.h>

typedef struct class_list {
    CSClass* _class;
    struct class_list* next;
} class_list_t; 

class_list_t* head = NULL;
class_list_t* tail = NULL;

CSClass* getClassByName(const char* name) {
    for (class_list_t* i = head; i; i = i->next) {
        if (strcmp(i->_class->name, name) == 0) {
            return i->_class;
        }
    }
    return NULL;
}

void registerClass(CSClass* _class) {
    class_list_t* newClass = malloc(sizeof(class_list_t));
    newClass->_class = _class;
    if (tail) {
        tail->next = newClass;
    }
    tail = newClass;
    if (head == NULL) {
        head = tail;
    }
}

CSObject* createObject(CSClass* __class__, int is_const) {
    CSObject* newObject = malloc(sizeof(CSObject));
    newObject->counterOfRef = 1;
    newObject->objectData = malloc(__class__->sizeOfObjectData);
    newObject->__class__ = __class__;
    newObject->is_const = is_const;
    return newObject;
}


void incref(CSObject* object) {
    if (object == NULL) return;
    ++object->counterOfRef;
}

void decref(CSObject* object) {
    if (object == NULL) return;
    if (object->counterOfRef == 0) return;
    --object->counterOfRef;
    if (object->counterOfRef == 0) {
        if (object->__class__->interface != NULL) {
            if (object->__class__->interface->__del__ != NULL) {
                object->__class__->interface->__del__(object);
            }
        }
        free(object->objectData);
        free(object);
    }
}

CSObject* add(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__add__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__add__(left, right);
    return new;
}

CSObject* mul(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__mul__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__mul__(left, right);
    return new;
}

CSObject* sub(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__sub__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__sub__(left, right);
    return new;
}

CSObject* _div(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__div__(left, right);
    return new;
}

CSObject* lt(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__lt__(left, right);
    return new;
}

CSObject* gt(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__gt__(left, right);
    return new;
}

CSObject* le(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__le__(left, right);
    return new;
}

CSObject* ge(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__ge__(left, right);
    return new;
}

CSObject* set(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__set__ == NULL) return NULL;
    if (left->is_const) return createExceptionObject("ConstError", NULL, 0, 0, "Try to assign a const object");
    CSObject* new = left->__class__->interface->__set__(left, right);
    return new;
}

CSObject* neg(CSObject* o) {
    
    if (o == NULL) return NULL;
    if (o->__class__ == NULL) return NULL;
    if (o->__class__->interface == NULL) return NULL;
    if (o->__class__->interface->__neg__ == NULL) return NULL;
    CSObject* new = o->__class__->interface->__neg__(o);
    return new;
}


CSObject* str(CSObject* o) {
    if (o == NULL) return createExceptionObject(
        "NullObjectError",
        NULL, 0, 0, "NullObject"
    );
    if (o->__class__ == NULL) return createExceptionObject(
        "NullClassError",
        NULL, 0, 0, "NullClassError"
    );
    if (o->__class__->interface == NULL) return createExceptionObject(
        "NullInterfaceError",
        NULL, 0, 0, "NullInterfaceError"
    );
    if (o->__class__->interface->__str__ == NULL) return createExceptionObject(
        "NullMethodError",
        NULL, 0, 0, "NullMethodError"
    );
    CSObject* new = o->__class__->interface->__str__(o);
    return new;
}


CSObject* call(CSObject* self, int argc, CSObject** argv) {
    if (self == NULL) return createExceptionObject(
        "NullObjectError",
        NULL, 0, 0, "NullObject"
    );
    if (self->__class__ == NULL) return createExceptionObject(
        "NullClassError",
        NULL, 0, 0, "NullClassError"
    );
    if (self->__class__->interface == NULL) return createExceptionObject(
        "NullInterfaceError",
        NULL, 0, 0, "NullInterfaceError"
    );
    if (self->__class__->interface->__call__ == NULL) return createExceptionObject(
        "NullMethodError",
        NULL, 0, 0, "NullMethodError"
    );
    CSObject* new = self->__class__->interface->__call__(self, argc, argv);
    return new;
}
