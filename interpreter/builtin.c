#include "object.h"

typedef struct BuiltinFunctionData {
    CSCallMethod __func;
}BuiltinFunctionData;

CSObject* builtinCall(CSObject* self, int argc, CSObject** argv) {
    TYPE_CHECK(self, "built-in");
    BuiltinFunctionData* data = self->objectData;
    return data->__func(self, argc, argv);
}

CSInterface builtinInterface = {
    .__call__ = builtinCall
};

CSClass builtinFunction = {
    .sizeOfObjectData = sizeof(BuiltinFunctionData),
    .name = "built-in",
    .interface = &builtinInterface
};

CSObject* createBuiltinFunction(CSCallMethod func) {
    CSObject* object = createObject(&builtinFunction, 1);
    BuiltinFunctionData* data = object->objectData;
    data->__func = func;
    return object;
}

CSClass* getBuiltinFuncClass() {
    return &builtinFunction;
}

void CSPrint(CSObject* self, int argc, CSObject** argv) {
    for (int i = 0; i < argc; ++i) {
        CSObject* _str = str(argv[i]);
        printf("%s ", getCStr(_str));
        decref(_str);
    }
}
