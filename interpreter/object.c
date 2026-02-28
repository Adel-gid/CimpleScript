#include "object.h"
#include <malloc.h>

CSObject* createObject(CSClass* __class__) {
    CSObject* newObject = malloc(sizeof(CSObject));
    newObject->counterOfRef = 1;
    newObject->objectData = malloc(__class__->sizeOfObjectData);
    newObject->__class__ = __class__;
    return newObject;
}


void incref(CSObject* object) {
    ++object->counterOfRef;
}

void decref(CSObject* object) {
    --object->counterOfRef;
    if (object->counterOfRef == 0) {
        if (object->__class__->interface->__del__ != NULL) {
            object->__class__->interface->__del__(object);
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
    decref(left);
    decref(right);
    return new;
}

CSObject* mul(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__mul__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__mul__(left, right);
    decref(left);
    decref(right);
    return new;
}

CSObject* sub(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__sub__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__sub__(left, right);
    decref(left);
    decref(right);
    return new;
}

CSObject* _div(CSObject* left, CSObject* right) {
    if (left == NULL || right == NULL) return NULL;
    if (left->__class__ == NULL) return NULL;
    if (left->__class__->interface == NULL) return NULL;
    if (left->__class__->interface->__div__ == NULL) return NULL;
    CSObject* new = left->__class__->interface->__div__(left, right);
    decref(left);
    decref(right);
    return new;
}

CSObject* neg(CSObject* o) {
    
    if (o == NULL) return NULL;
    if (o->__class__ == NULL) return NULL;
    if (o->__class__->interface == NULL) return NULL;
    if (o->__class__->interface->__neg__ == NULL) return NULL;
    CSObject* new = o->__class__->interface->__neg__(o);
    decref(o);
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
    decref(o);
    return new;
}


