
#include "../parser/parser.h"

void CSPrint(CSObject* self, int argc, CSObject** argv);

int main(int argc, char *argv[])
{
    registerClass(getBoolClass());
    registerClass(getStringClass());
    registerClass(getDoubleClass());
    registerClass(getBuiltinFuncClass());
    CSScope globalScope = {
        0
    };
    CSObject* printFunc = createBuiltinFunction(CSPrint);
    scopeCreate(&globalScope, "print", "built-in");
    scopeSet(&globalScope, "print", printFunc);
    char* input = "{\n"
    "    number:double = 20\n"
    "    while number > 0 {\n"
    "        print(number)\n"
    "        number = number - 1\n"
    "    }\n"
    "}";
    stmt_t* stmt = parse_code(input);
    //print_tree_stmt(stmt, 0);
    execStmt(&globalScope, stmt);
    free_stmt(stmt);
    return 0;
}
