#include "object.h"
#include <malloc.h>
#include <stdio.h>

CSObject* scopeGet(CSScope* currentScope, const char* name) {
    while (currentScope) {
        CSVar* current = currentScope->vars;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                incref(current->object);
                return current->object;
            }
            current = current->next;
        }
        currentScope = currentScope->parent; // Ищем во внешней области
    }
    return NULL; // Ошибка: переменная не найдена
}

CSObject* scopeCreate(CSScope* currentScope, const char* name, const char* className) {
    CSObject* newObject = createObject(getClassByName(className), 0);
    CSVar* newVar = malloc(sizeof(CSVar));
    newVar->name = strdup(name);
    newVar->object = newObject;
    newVar->next = currentScope->vars;
    currentScope->vars = newVar;
    return newObject;
}

void scopeClean(CSScope* scope) {
    CSVar* current = scope->vars;
    while (current) {
        decref(current->object);
        CSVar* temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }
}

CSObject* scopeSet(CSScope* currentScope, const char* name, CSObject* object) {

    while (currentScope) {
        CSVar* current = currentScope->vars;
        while (current) {
            if (strcmp(current->name, name) == 0) {
                incref(object);          // Захватываем новое
                current->object = object;
                decref(current->object); // Освобождаем старое значение
                return object;
            }
            current = current->next;
        }
        currentScope = currentScope->parent;
    }
    return NULL;
}