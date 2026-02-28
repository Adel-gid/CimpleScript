#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "object.h"


CSObject* double_add(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld + *rd);
}

CSObject* double_sub(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld - *rd);
}

CSObject* double_mul(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld * *rd);
}

CSObject* double_div(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createDoubleObject(*ld / *rd);
}


CSObject* double_lt(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createBoolObject(*ld < *rd);
}

CSObject* double_gt(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    return createBoolObject(*ld > *rd);
}

CSObject* double_neg(CSObject* o) {
    double* ld = o->objectData;
    return createDoubleObject(-(*ld));
}


int countAfterDecimalPoint(double f) {

    double afterPoint = f - floor(f);
    int n = 0;
    afterPoint *= 10;
    while (afterPoint) {    
        ++n;
        afterPoint -= floor(afterPoint);
        afterPoint *= 10;
    }   
    return n;

}

CSObject* double_str(CSObject* o) {
    TYPE_CHECK(o, "double");
    char buff[16];
    double* ld = o->objectData;
    int n = snprintf(buff, 16, "%.*f", countAfterDecimalPoint(*ld), *ld);
    return createStringObject(buff);
}

CSObject* double_set(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "double");
    TYPE_CHECK(right, "double");
    double* ld = left->objectData;
    double* rd = right->objectData;
    *ld = *rd;
    return left;
}

CSInterface double_interface = {
    .__add__ = double_add,
    .__mul__ = double_mul,
    .__neg__ = double_neg,
    .__str__ = double_str,
    .__div__ = double_div,
    .__sub__ = double_sub,
    .__lt__  = double_lt,
    .__gt__  = double_gt,
    .__set__ = double_set
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
    CSObject* object = createObject(&double_class, 1);
    double* _data = object->objectData;
    *_data = i;
    return object;
}

