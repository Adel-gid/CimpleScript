
#include "../parser/parser.h"

int main(int argc, char *argv[])
{
    char* input = "while (i < 4) {\n"
    "4 + 45 \n"
    "23*4\n"
    "if (i > 3) 45 * 5"
    "else {\n"
    "76-467"
    "}\n"
    "}\n";
    stmt_t* stmt = parse_code(input);
    print_tree_stmt(stmt, 0);
    free_stmt(stmt);
    return 0;
}
