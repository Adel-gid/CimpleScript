#include <stdio.h>
#include "compile.h"
#include "../parser/parser.h"

void print_ir(ir_t* ir);

int main(int argc, char* argv[]) {
    registerClass(getInt32Class());
    char* input = "function main() {\n"
    "number: int32 = 0\n"
    "number1: int32 = 1\n"
    "while number < 999 {\n"
    "print(number)\n"
    "number = number + 1\n"
    "number1 = number * number\n"
    "}\n"
    "}";
    top_level_stmt_t* stmt = parse_code(input);
    if (stmt->kind == top_level_expr && stmt->expr->kind == expr_error) {
        print_tree_top_stmt(stmt, 0);
        return 1;
    }
    ir_t ir = { 0, 0};
    codegen(&ir, stmt);
    print_ir(&ir);
    free_top_stmt(stmt);
    return 0;
}