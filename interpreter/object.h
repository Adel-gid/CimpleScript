#ifndef __CS_OBJECT_H__
#define __CS_OBJECT_H__

#include <stdint.h>
#include <stddef.h>

typedef struct CSObject CSObject;

typedef CSObject*(*CSAddMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSMulMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSSubMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSDivMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSNegMethod)(CSObject*);
typedef CSObject*(*CSStrMethod)(CSObject*);
typedef CSObject*(*CSLessThanMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSGreaterThanMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSLessOrEqualMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSGreaterOrEqualMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSDeleteMethod)(CSObject*);

typedef struct CSInterface {
    CSAddMethod           __add__;
    CSMulMethod           __mul__;
    CSSubMethod           __sub__;
    CSDivMethod           __div__;
    CSNegMethod           __neg__;
    CSStrMethod           __str__;
    CSLessThanMethod       __lt__;
    CSLessOrEqualMethod    __le__;
    CSGreaterThanMethod    __gt__;
    CSGreaterOrEqualMethod __ge__;
    CSDeleteMethod         __del__;
} CSInterface;

typedef struct CSClass {
    const char* name;
    CSInterface* interface;
    void* classData;
    size_t sizeOfObjectData;
} CSClass;

typedef struct CSObject {
    void* objectData;
    CSClass* __class__;
    size_t counterOfRef;
} CSObject;

CSObject* createObject(CSClass* __class__);

CSObject* createDoubleObject(double i);
CSObject* createStringObject(const char* str);
CSObject* createExceptionObject(
    const char* exceptionName, 
    const char* source, int line, int column, 
    const char* description
);

CSClass* getDoubleClass();
CSClass* getStringClass();

const char* getCStr(CSObject* object);

void incref(CSObject* object);
void decref(CSObject* object);

CSObject* add(CSObject* left, CSObject* right);
CSObject* mul(CSObject* left, CSObject* right);
CSObject* sub(CSObject* left, CSObject* right);
CSObject* neg(CSObject* o);
CSObject* str(CSObject* o);
CSObject* _div(CSObject* left, CSObject* right);

#define TYPE_CHECK(x, className) do {if (strcmp((x)->__class__->name, className) != 0) {\
    return createExceptionObject("TypeError", NULL, 0, 0, "Error type of object");}} while (0)

#endif