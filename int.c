#include <stdint.h>
#include "object.h"


CSObject* double_add(CSObject* left, CSObject* right) {
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld + *rd);
}

CSObject* double_mul(CSObject* left, CSObject* right) {
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld * *rd);
}

CSObject* double_neg(CSObject* o) {
    double* ld = o->objectData;
    return createDoubleObject(-(*ld));
}

CSObject* double_str(CSObject* o) {
    double* ld = o->objectData;
    return createDoubleObject(-(*ld));
}

CSInterface double_interface = {
    .__add__ = double_add,
    .__mul__ = double_mul,
    .__neg__ = double_neg
};

CSClass double_class = {
    .interface = &double_interface,
    .name = "double",
    .sizeOfObjectData = 8
};

CSClass* getDoubleClass() {
    return &double_class;
}

CSObject* createDoubleObject(double i) {
    CSObject* object = createObject(&double_class);
    double* _data = object->objectData;
    *_data = i;
    return object;
}

