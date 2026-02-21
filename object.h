#ifndef __CS_OBJECT_H__
#define __CS_OBJECT_H__

#include <stdint.h>
#include <stddef.h>

typedef struct CSObject CSObject;

typedef CSObject*(*CSAddMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSMulMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSNegMethod)(CSObject*);
typedef CSObject*(*CSStrMethod)(CSObject*);

typedef struct CSInterface {
    CSAddMethod __add__;
    CSMulMethod __mul__;
    CSNegMethod __neg__;
    CSStrMethod __str__;
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

CSClass* getDoubleClass();
CSClass* getStringClass();

void incref(CSObject* object);
void decref(CSObject* object);

CSObject* add(CSObject* left, CSObject* right);
CSObject* mul(CSObject* left, CSObject* right);
CSObject* neg(CSObject* o);
CSObject* str(CSObject* o);


#endif