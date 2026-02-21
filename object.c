#include "object.h"

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
CSObject* neg(CSObject* o) {
    if (o == NULL) return NULL;
    CSObject* new = o->__class__->interface->__neg__(o);
    decref(o);
    return new;
}


CSObject* str(CSObject* o) {
    if (o == NULL) return NULL;
    CSObject* new = o->__class__->interface->__str__(o);
    decref(o);
    return new;
}
