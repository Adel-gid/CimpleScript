#include "object.h"

typedef struct BoolData {
    int _boolean;
} BoolData;

CSClass boolClass = {
    .sizeOfObjectData = sizeof(BoolData),
    .name = "bool"
};

int toCBool(CSObject* object) {
    if (object == NULL) return 0;
    TYPE_CHECK(object, "bool");
    BoolData* boolData = object->objectData;
    return boolData->_boolean;
}

CSClass* getBoolClass() {
    return &boolClass;
}

CSObject* createBoolObject(int _boolean) {
    CSObject* object = createObject(&boolClass, 1);
    BoolData* boolData = object->objectData;
    boolData->_boolean = _boolean;
    return object;
}