#ifndef __CS_OBJECT_H__
#define __CS_OBJECT_H__

#include <stdint.h>
#include <stddef.h>

#include "../parser/parser.h"

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
typedef CSObject*(*CSEqualMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSDeleteMethod)(CSObject*);
typedef CSObject*(*CSCallMethod)(CSObject*, int, CSObject**);
typedef CSObject*(*CSSetMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSGetIndexMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSSetIndexMethod)(CSObject*, CSObject*, CSObject*);
typedef CSObject*(*CSGetAttributeMethod)(CSObject*, CSObject*);
typedef CSObject*(*CSSetAttributeMethod)(CSObject*, CSObject*, CSObject*);

typedef struct CSInterface {
    CSAddMethod                __add__;
    CSMulMethod                __mul__;
    CSSubMethod                __sub__;
    CSDivMethod                __div__;
    CSNegMethod                __neg__;
    CSStrMethod                __str__;
    CSLessThanMethod            __lt__;
    CSLessOrEqualMethod         __le__;
    CSGreaterThanMethod         __gt__;
    CSGreaterOrEqualMethod      __ge__;
    CSDeleteMethod             __del__;
    CSCallMethod              __call__;
    CSSetMethod                __set__;
    CSEqualMethod               __eq__;
    CSGetIndexMethod      __getindex__;
    CSSetIndexMethod      __setindex__;
    CSGetAttributeMethod   __getattr__;
    CSSetAttributeMethod   __setattr__;
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
    int is_const;
} CSObject;

CSObject* createObject(CSClass* __class__, int is_const);

CSObject* createDoubleObject(double i);
CSObject* createStringObject(const char* str);
CSObject* createBoolObject(int _boolean);
CSObject* createExceptionObject(
    const char* exceptionName, 
    const char* source, int line, int column, 
    const char* description
);

CSClass* getDoubleClass();
CSClass* getIntClass();
CSClass* getUIntClass();
CSClass* getArrayClass();
CSClass* getStringClass();
CSClass* getBoolClass();
CSClass* getBuiltinFuncClass();
CSClass* getFunctionClass();

const char* getCStr(CSObject* object);

void incref(CSObject* object);
void decref(CSObject* object);

CSObject* add(CSObject* left, CSObject* right);
CSObject* mul(CSObject* left, CSObject* right);
CSObject* sub(CSObject* left, CSObject* right);
CSObject* neg(CSObject* o);
CSObject* str(CSObject* o);
CSObject* _div(CSObject* left, CSObject* right);
CSObject* lt(CSObject* left, CSObject* right);
CSObject* gt(CSObject* left, CSObject* right);
CSObject* le(CSObject* left, CSObject* right);
CSObject* ge(CSObject* left, CSObject* right);
CSObject* eq(CSObject* left, CSObject* right);
CSObject* set(CSObject* left, CSObject* right);
CSObject* getIndex(CSObject* self, CSObject* index);
CSObject* setIndex(CSObject* self, CSObject* index, CSObject* toInsert);
CSObject* getAttr(CSObject* self, CSObject*);
CSObject* setAttr(CSObject* self, CSObject*, CSObject*);

#define TYPE_CHECK(x, className) do {if (strcmp((x)->__class__->name, className) != 0) {\
    return createExceptionObject("TypeError", NULL, 0, 0, "Error type of object");}} while (0)

typedef struct CSVar {
    const char* name;
    CSObject* object;
    struct CSVar* next;
} CSVar;

typedef struct CSScope {
    CSVar* vars;
    struct CSScope* parent;
} CSScope;

CSClass* getExceptionClass();

CSClass* getClassByName(const char* name);
void registerClass(CSClass* _class);

CSObject* call(CSObject* self, int argc, CSObject** argv);

CSObject* scopeGet(CSScope* currentScope, const char* name);
CSObject* scopeCreate(CSScope* currentScope, const char* name, const char* className);
CSObject* scopeSet(CSScope* currentScope, const char* name, CSObject* object);
void scopeClean(CSScope* scope);
CSObject* createBuiltinFunction(CSCallMethod func);

int toCBool(CSObject* object);


CSObject* execExpr(CSScope* scope, expr_t* expr);
CSObject* execStmt(CSScope* scope, stmt_t* stmt);
void      execTopLevelStmt(CSScope* scope, top_level_stmt_t* top_level);

#endif