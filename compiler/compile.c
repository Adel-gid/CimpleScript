#include "compile.h"
#include <string.h>
#include <malloc.h>

typedef struct class_list {
    class_t* _class;
    struct class_list* next;
} class_list_t; 

class_list_t* head = NULL;
class_list_t* tail = NULL;

class_t* getClassByName(const char* name) {
    for (class_list_t* i = head; i; i = i->next) {
        if (strcmp(i->_class->name, name) == 0) {
            return i->_class;
        }
    }
    return NULL;
}

void registerClass(class_t* _class) {
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

object_t* createObject(class_t* _class, location_t loc) {
    object_t* result = malloc(sizeof(object_t));
    result->_class = _class;
    result->loc = loc;
    return result;
}

void freeObject(object_t* obj) {
    free(obj);
}

object_t* scopeGet(CSScope* currentScope, const char* name) {
    while (currentScope) {
        CSVar* current = currentScope->vars;
        while (current) {
            if (strcmp(current->name, name) == 0) {;
                return current->object;
            }
            current = current->next;
        }
        currentScope = currentScope->parent; // Ищем во внешней области
    }
    return NULL; // Ошибка: переменная не найдена
}

object_t* scopeCreate(CSScope* currentScope, const char* name, const char* className, location_t loc) {
    object_t* object = createObject(getClassByName(className), loc);
    CSVar* newVar = malloc(sizeof(CSVar));
    newVar->name = strdup(name);
    newVar->object = object;
    newVar->next = currentScope->vars;
    currentScope->vars = newVar;
    return object;
}

object_t* scopeSet(CSScope* currentScope, const char* name, object_t* object) {
    while (currentScope) {
        CSVar* current = currentScope->vars;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                free(current->object);
                current->object = object;
                return object;
            }
            current = current->next;
        }
        currentScope = currentScope->parent;
    }
    return NULL;
}

void scopeClean(CSScope* scope) {
    CSVar* current = scope->vars;
    while (current) {
        free(current->object);
        CSVar* temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }
}


int add(char* buff, int buffsize, object_t* l, object_t* r) {
    if (l == NULL) return -ENOSYS;
    if (l->_class == NULL) return -ENOSYS;
    if (l->_class->interface == NULL) return -ENOSYS;
    if (l->_class->interface->add == NULL) return -ENOSYS;
    return l->_class->interface->add(buff, buffsize, l, r);
}

int sub(char* buff, int buffsize, object_t* l, object_t* r) {
    if (l == NULL) return -ENOSYS;
    if (l->_class == NULL) return -ENOSYS;
    if (l->_class->interface == NULL) return -ENOSYS;
    if (l->_class->interface->sub == NULL) return -ENOSYS;
    return l->_class->interface->sub(buff, buffsize, l, r);
}

location_t getArgLoc(int argNo) {

    location_t loc = { 0 };
    switch (argNo)
    {
    case 1:
        loc.regNo = 2;
        loc.kind = loc_reg;
        break;
    case 2:
        loc.regNo = 3;
        loc.kind = loc_reg;
    
    default:
        break;
    }
    return loc;

}