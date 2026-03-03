#include "object.h"

typedef struct ExceptionData {
    const char* exceptionName;
    const char* source; int line; int column; 
    const char* description
} ExceptionData;

CSClass exceptionClass = {
    .sizeOfObjectData = sizeof(ExceptionData),
    .name = "exception",
    .interface = NULL
};

CSClass* getExceptionClass() {
    return &exceptionClass;
}

CSObject* createExceptionObject(
    const char* exceptionName, 
    const char* source, int line, int column, 
    const char* description
) {
    CSObject* result = createObject(&exceptionClass, 1);
    ExceptionData* exceptData = result->objectData;
    exceptData->exceptionName = exceptionName;
    exceptData->source = source;
    exceptData->line = line;
    exceptData->column = column;
    exceptData->description = description;
    return result;
}