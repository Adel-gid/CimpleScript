#include "object.h"
#include "../parser/parser.h"

typedef struct FunctionData {
    int argc;
    stmt_t* stmt;
    CSScope scope;
    CSObject* result;
} FunctionData;


CSObject* functionCall(CSObject* self, int argc, CSObject** argv) {
    FunctionData* data = self->objectData;
    CSVar* vars = data->scope.vars;
    CSScope callScope;
    callScope.parent = data->scope.parent; // Замыкание
    callScope.vars = NULL;
    for (int i = data->argc; i > 0; --i, --argc) {
        CSObject* newVar = scopeCreate(&callScope, vars->name, vars->object->__class__->name);
        CSObject* toInsert = NULL;
        if (argc) {
            toInsert = argv[argc-1];
        }
        else {
            toInsert = vars->object;
        }
        set(newVar, toInsert);
        vars = vars->next;
    }
    data->result = execStmt(&callScope, data->stmt);
    scopeClean(&callScope);
    return data->result;
}

CSInterface functionInterface = {
    .__call__ = functionCall
};

CSClass functionClass = {
    .name = "function",
    .sizeOfObjectData = sizeof(FunctionData),
    .interface = &functionInterface
};

CSClass* getFunctionClass() {
    return &functionClass;
}
