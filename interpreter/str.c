#include "object.h"
#include <string.h>
#include <malloc.h>

typedef struct StringObject {
    const char* __cstr;
} StringObject;

CSObject* strDel(CSObject* obj) {
    TYPE_CHECK(obj, "str");
    StringObject* objData = obj->objectData;
    free(objData->__cstr);
    return NULL;
}

CSObject* strAdd(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "str");
    TYPE_CHECK(right, "str");
    const char* cstr1 = getCStr(left);
    const char* cstr2 = getCStr(right);
    int newSize = strlen(cstr1) + strlen(cstr2)+1;
    char* _str = malloc(newSize);
    strcpy(_str, cstr1);
    strcat(_str, cstr2);
    CSObject* newStr = createStringObject(_str);
    free(_str);
    return newStr;
}

CSObject* strSet(CSObject* left, CSObject* right) {
    TYPE_CHECK(left, "str");
    TYPE_CHECK(right, "str");
    StringObject* objData1 = left->objectData;
    StringObject* objData2 = right->objectData;
    free(objData1->__cstr);
    objData1->__cstr = strdup(objData2->__cstr);
    return left;
}

CSObject* strStr(CSObject* _str) {
    return _str;
}

CSInterface strInterface = {
    .__add__ = strAdd,
    .__str__ = strStr
};

CSClass stringClass = {
    .sizeOfObjectData = sizeof(StringObject),
    .name = "str",
    .interface = &strInterface
};

CSClass* getStringClass() {
    return &stringClass;
}

CSObject* createStringObject(const char* str) {

    CSObject* result = createObject(&stringClass, 1);
    StringObject* objData = result->objectData;
    objData->__cstr = strdup(str);
    return result;

}

const char* getCStr(CSObject* object) {
    if (object == NULL) return "(null)";
    TYPE_CHECK(object, "str");
    StringObject* str = object->objectData;
    return str->__cstr;
}

