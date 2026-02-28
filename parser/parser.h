#ifndef __PARSER_H__
#define __PARSER_H__

#include "../interpreter/object.h"

typedef struct expr expr_t;

typedef struct binop {
    int op;
    expr_t* left;
    expr_t* right;
} binop_t;

typedef struct unop {
    int op;
    expr_t* expr;
} unop_t;

typedef struct expr_list {
    expr_t* expr;
    struct expr_list* next;
} expr_list_t;

typedef struct expr {
    int kind;
    union {
        binop_t             binop;
        unop_t               unop;
        expr_list_t*    expr_list;
        double             number;
        const char*            id;
    };
} expr_t;

enum ExprKind {
    expr_id,
    expr_num,
    expr_unop,
    expr_binop,
    expr_list
};

typedef struct stmt stmt_t;

typedef struct stmt_list {
    stmt_t* stmt;
    struct stmt_list* next;
} stmt_list_t;

typedef struct while_stmt {
    expr_t* condition;
    stmt_t* stmt;
} while_stmt;

typedef struct if_stmt {
    expr_t* condition;
    stmt_t* stmt;
    stmt_t* elseStmt;
} if_stmt;

typedef struct stmt {
    int kind;
    union {
        expr_t* expr;
        stmt_list_t* stmt_list;
        while_stmt while_stmt;
        if_stmt    if_stmt;
    };
} stmt_t;

enum StmtKind {
    stmt_expr,
    stmt_list,
    stmt_while,
    stmt_if
};

void free_expr(expr_t* expr);
int print_tree_expr(expr_t* expr, int tabs);
int print_tree_stmt(stmt_t* stmt, int tabs);
void free_stmt(stmt_t* stmt);

stmt_t* parse_code(char* input);

CSObject* execExpr(CSScope* scope, expr_t* expr);
CSObject* execStmt(CSScope* scope, stmt_t* stmt);

#endif