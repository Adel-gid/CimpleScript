
#include "../parser/parser.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

CSObject* CSGetAddr(CSObject* self, int argc, CSObject** argv);
void CSPrint(CSObject* self, int argc, CSObject** argv);

int earlyInit(CSScope* globalScope) {
    registerClass(getBoolClass());
    registerClass(getStringClass());
    registerClass(getDoubleClass());
    registerClass(getBuiltinFuncClass());
    registerClass(getFunctionClass());

    CSObject* printFunc = createBuiltinFunction(CSPrint);
    CSObject* addrFunc = createBuiltinFunction(CSGetAddr);
    scopeCreate(globalScope, "print", "built-in");
    scopeCreate(globalScope, "addr", "built-in");
    scopeSet(globalScope, "print", printFunc);
    scopeSet(globalScope, "addr", addrFunc);
    return 0;
}

int execFile(const char* filename) {
    FILE* in = fopen(filename, "rb");
    fseek(in, 0, SEEK_END);
    long sizeOfFile = ftell(in);
    fseek(in, 0, SEEK_SET);
    char* fileData = malloc(sizeOfFile+1);
    memset(fileData, 0, sizeOfFile);
    fread(fileData, 1, sizeOfFile, in);
    fileData[sizeOfFile] = '\0';
    CSScope globalScope = {
        .vars = NULL, .parent = NULL
    };
    earlyInit(&globalScope);
    char* input = fileData;
    top_level_stmt_t* stmt = parse_code(input);
    free(fileData);
    execTopLevelStmt(&globalScope, stmt);
    CSObject* mainFunc = scopeGet(&globalScope, "main");
    CSObject* result = call(mainFunc, 0, NULL);
    CSPrint(NULL, 1, &result);
    free_top_stmt(stmt);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("not enough arguments\n");
        return 1;
    }
    char* filename = NULL;
    for (char** i = argv; i < argc + argv; ++i) {
        if (strcmp(*i, "-v") == 0 || strcmp(*i, "--version") == 0) {
            printf("CimpleScript interpreter. Version: 0.0.1\n");
            continue;
        }
        filename = *i;
    }
    if (filename) {
        return execFile(filename);
    }
    return 1;
}
