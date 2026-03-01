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
        expr_t* _return_stmt;
    };
} stmt_t;

enum StmtKind {
    stmt_expr,
    stmt_list,
    stmt_while,
    stmt_if,
    stmt_return,
};


enum token {
    tok_unknown,
    tok_eol,
    tok_eof,
    tok_id,
    tok_num,
    tok_lpar, // (
    tok_rpar, // )
    tok_flpar, // {
    tok_frpar, // }
    tok_str,
    tok_while,
    tok_for,
    tok_function,
    tok_class,
    tok_if,
    tok_else,
    tok_add,
    tok_sub,
    tok_mul,
    tok_div,
    tok_arrow, //->
    tok_point, //.
    tok_lt, //<
    tok_gt, //>
    tok_le, //<=
    tok_ge, //>=
    tok_eq, //==
    tok_assign, //=
    tok_colon, //:
    tok_semicolon, //;
    tok_comma, //,
    tok_return, // return keyword
};

typedef struct function_def_stmt {
    expr_t* definition;
    stmt_t* body;
} function_def_stmt_t;

typedef struct top_level_stmt top_level_stmt_t;

typedef struct top_level_stmt_list {
    top_level_stmt_t* _stmt;
    struct top_level_stmt_list* next;
} top_level_stmt_list_t;

typedef struct top_level_stmt {
    int kind;
    union {
        function_def_stmt_t func_def;
        expr_t* expr;
        top_level_stmt_list_t* list;
    };
} top_level_stmt_t;

enum TopLevelKind {
    top_level_expr,
    top_level_func,
    top_level_list
};

void free_expr(expr_t* expr);
int print_tree_expr(expr_t* expr, int tabs);
int print_tree_stmt(stmt_t* stmt, int tabs);
int print_tree_top_stmt(top_level_stmt_t* top_stmt, int tabs);
void free_stmt(stmt_t* stmt);

top_level_stmt_t* parse_code(char* input);

CSObject* execExpr(CSScope* scope, expr_t* expr);
CSObject* execStmt(CSScope* scope, stmt_t* stmt);
void      execTopLevelStmt(CSScope* scope, top_level_stmt_t* top_level);

#endif