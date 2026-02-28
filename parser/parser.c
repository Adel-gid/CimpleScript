#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "../interpreter/object.h"
#include "parser.h"

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
};

char* tempString = NULL;

struct seq {
    const char* _str;
    int tok;
};

struct seq keywords[] = {
    {"while", tok_while},
    {"for", tok_for},
    {"function", tok_function},
    {"class", tok_class},
    {"if", tok_if},
    {"else", tok_else}
};

struct seq seqs[] = {
    {"\n", tok_eol},
    {"\r", tok_eol},
    {"+", tok_add},
    {"-", tok_sub},
    {"/", tok_div},
    {"*", tok_mul},
    {"(", tok_lpar},
    {")", tok_rpar},
    {"->", tok_arrow}, //->
    {".", tok_point}, //.
    {"<", tok_lt}, //<
    {">", tok_gt}, //>
    {"<=", tok_le}, //<=
    {">=", tok_ge}, //>=
    {"==", tok_eq}, //==
    {"=", tok_assign}, //=
    {"{", tok_flpar},
    {"}", tok_frpar}
};

int gettok(char* str, char** endp) {
    while (*str && isspace(*str)) ++str;
    if (*str == '\0') {
        *endp = NULL;
        return tok_eof;
    }
    for (struct seq* i = keywords; i < keywords + sizeof(keywords)/sizeof(*keywords); ++i) {
        int len = strlen(i->_str);
        if (strncmp(str, i->_str, len) == 0) {
            str += len;
            *endp = str;
            return i->tok;
        }
    }
    if (isdigit(*str)) {
        int n = 0;
        for (; isdigit(*str) && *str; ++n, ++str);
        tempString = malloc(n+1);
        memcpy(tempString, str-n, n);
        tempString[n] = '\0';
        *endp = str;
        return tok_num;
    }
    if (isalpha(*str) || *str == '_') {
        int n = 0;
        for (; (isalnum(*str) || *str == '_') && *str; ++n, ++str);
        tempString = malloc(n+1);
        memcpy(tempString, str-n, n);
        tempString[n] = '\0';
        *endp = str;
        return tok_id;
    }
    if (*str == '"' || *str == '\"') {
        int n = 0;
        int quote_mark = *str;
        ++str;
        for (; *str != quote_mark && *str != '\n' && *str != '\r' && *str; ++n, ++str);
        tempString = malloc(n+1);
        memcpy(tempString, str-n, n);
        tempString[n] = '\0';
        *endp = str;
        return tok_str;
    }
    if (*str == '/') {
        ++str;
        if (*str == '/') {
            while (*str && *str != '\n') ++str;
            return gettok(str, endp);
        }
        --str; //it's div operator
    }
    for (struct seq* i = seqs; i < seqs + sizeof(seqs)/sizeof(*seqs); ++i) {
        int len = strlen(i->_str);
        if (strncmp(str, i->_str, len) == 0) {
            str += len;
            *endp = str;
            return i->tok;
        }
    }
    return tok_unknown;
}



expr_t* parse_expr(char** str);

int CurTok = 0;

void free_expr(expr_t* expr);

int getNextToken(char** pos) {
    return CurTok = gettok(*pos, pos);
}

expr_t* parse_number(char** pos) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = expr_num;
    expr->number = strtod(tempString, NULL);
    free(tempString);
    return expr;
}


expr_t* parse_id(char** pos) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = expr_id;
    expr->id = tempString;       
    return expr;
}
expr_t* parse_parenth(char** pos);


expr_t* parse_parenth(char** pos) {
    getNextToken(pos);            
    expr_t* expr = parse_expr(pos);
    if (!expr) return NULL;
    if (CurTok != tok_rpar) {
        free_expr(expr);
        return NULL;
    }
    getNextToken(pos);            
    return expr;
}

expr_t* parse_primary(char** pos);


expr_t* parse_unop(char** pos) {
    int op = CurTok;
    getNextToken(pos);           
    expr_t* prim = parse_primary(pos);
    if (!prim) return NULL;
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = expr_unop;
    expr->unop.op = op;
    expr->unop.expr = prim;
    return expr;
}

expr_t* parse_primary(char** pos) {
    expr_t* expr = NULL;
    switch (CurTok) {
        case tok_num:
            expr = parse_number(pos);
            getNextToken(pos);    
            break;
        case tok_id:
            expr = parse_id(pos);
            getNextToken(pos);   
            break;
        case tok_lpar:
            expr = parse_parenth(pos);
            break;
        case tok_add:
        case tok_sub:
        case tok_mul:             
            expr = parse_unop(pos);
            break;
        default:
            return NULL;
    }
    return expr;
}


int getTokPrec(int tok) {
    switch (tok) {
        case tok_lt: case tok_gt: case tok_le: case tok_ge: case tok_eq: 
            return 5; // Приоритет ниже, чем у сложения
        case tok_add: case tok_sub: 
            return 10;
        case tok_mul: case tok_div: 
            return 20;
        default: 
            return -1;
    }
}

expr_t* parse_binop(char** pos, int prevPrec, expr_t* lhs) {
    while (1) {
        int curPrec = getTokPrec(CurTok);
        if (curPrec < prevPrec) return lhs;

        int op = CurTok;
        getNextToken(pos);                
        expr_t* rhs = parse_primary(pos);
        if (!rhs) {
            free_expr(lhs);
            return NULL;
        }

        int nextPrec = getTokPrec(CurTok);
        if (curPrec < nextPrec) {
            rhs = parse_binop(pos, curPrec + 1, rhs);
            if (!rhs) {
                free_expr(lhs);
                return NULL;
            }
        }

        expr_t* node = malloc(sizeof(expr_t));
        node->kind = expr_binop;
        node->binop.op = op;
        node->binop.left = lhs;
        node->binop.right = rhs;
        lhs = node;
    }
}


expr_t* parse_expr(char** pos) {
    expr_t* lhs = parse_primary(pos);
    if (!lhs) return NULL;
    return parse_binop(pos, 0, lhs);
}

stmt_t* parse_stmt(char** input);

stmt_t* parse_code(char* str) {
    char* pos = str;
    getNextToken(&pos);
    return parse_stmt(&pos);
}

stmt_t* parse_expr_stmt(char** str) {
    stmt_t* newStmt = malloc(sizeof(stmt_t));
    newStmt->kind = stmt_expr;
    newStmt->expr = parse_expr(str);
    return newStmt;
}

stmt_t* parse_stmt_list(char** str) {
    printf("CurTok %d\n", CurTok);
    getNextToken(str);
    stmt_t* newStmt = NULL;
    stmt_list_t* stmtListLast = NULL;
    stmt_list_t* stmtListResult = NULL;
    while (CurTok != tok_frpar) {
        while (CurTok == tok_eol) getNextToken(str);
        if (CurTok == tok_frpar) break;
        newStmt = parse_stmt(str);
        printf("CurTok %d\n", CurTok);
        printf("newStmt:\n");
        if (newStmt)
            print_tree_stmt(newStmt, 4);

        stmt_list_t* newStmtList = malloc(sizeof(stmt_list_t));
        newStmtList->next = NULL;
        newStmtList->stmt = newStmt;
        if (stmtListLast != NULL) {
            stmtListLast->next = newStmtList;
        }
        stmtListLast = newStmtList;
        if (stmtListResult == NULL) {
            stmtListResult = stmtListLast;
        }
    }
    stmt_t* result = malloc(sizeof(stmt_t));
    result->kind = stmt_list;
    result->stmt_list = stmtListResult;
    return result;
}

stmt_t* parse_while_stmt(char** str) {
    getNextToken(str);
    expr_t* cond = parse_parenth(str);
    stmt_t* _true_stmt = parse_stmt(str);
    stmt_t* result = malloc(sizeof(stmt_t));
    result->kind = stmt_while;
    result->while_stmt.condition = cond;
    result->while_stmt.stmt = _true_stmt;
    return result;
}

stmt_t* parse_if_stmt(char** str) {
    getNextToken(str);
    expr_t* cond = parse_expr(str);
    stmt_t* _true_stmt = parse_stmt(str);
    stmt_t* _false_stmt = NULL;
    if (CurTok == tok_else) {
        getNextToken(str);
        _false_stmt = parse_stmt(str);
    }
    stmt_t* result = malloc(sizeof(stmt_t));
    result->kind = stmt_if;
    result->if_stmt.condition = cond;
    result->if_stmt.stmt = _true_stmt;
    result->if_stmt.elseStmt = _false_stmt;
    return result;
}

stmt_t* parse_stmt(char** str) {
    switch (CurTok)
    {
    case tok_while:
        return parse_while_stmt(str);
        break;
    case tok_if:
        return parse_if_stmt(str);
    case tok_flpar:
        return parse_stmt_list(str);
    default:
        break;
    }
    return parse_expr_stmt(str);
}

int print_tree_expr(expr_t* expr, int tabs) {
    for (int i = 0;i < tabs;++i) putchar(' ');
    switch (expr->kind)
    {
    case expr_id:
        printf("id: %s\n", expr->id);
        break;
    case expr_num:
        printf("num: %f\n", expr->number);
        break;
    case expr_unop:
        printf("unop: %d\n", expr->unop.op);
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("expr:\n");
        return print_tree_expr(expr->unop.expr, tabs+4);
    case expr_binop:
        printf("binop: %d\n", expr->binop.op);
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("left:\n");
        print_tree_expr(expr->binop.left, tabs+4);
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("right:\n");
        print_tree_expr(expr->binop.right, tabs+4);
        break;
    default:
        break;
    }
    return 0;
}

int print_tree_stmt(stmt_t* stmt, int tabs) {
    for (int i = 0;i < tabs;++i) putchar(' ');
    if (stmt == NULL) printf("(null)\n");
    switch (stmt->kind)
    {
    case stmt_expr:
        printf("expr:\n");
        return print_tree_expr(stmt->expr, tabs+4);
        break;
    case stmt_if:
        printf("if:\n");
        print_tree_expr(stmt->if_stmt.condition, tabs+4);
        print_tree_stmt(stmt->if_stmt.stmt, tabs+4);
        if (stmt->if_stmt.elseStmt) {
            printf("else:\n");
            print_tree_stmt(stmt->if_stmt.elseStmt, tabs+4);
        }
        break;
    case stmt_while:
        printf("while:\n");
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("condition:\n");
        print_tree_expr(stmt->while_stmt.condition, tabs+4);
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("statement:\n");
        print_tree_stmt(stmt->while_stmt.stmt, tabs+4);
        break;
    case stmt_list:
        printf("{\n");
        for (stmt_list_t* i = stmt->stmt_list; i; i = i->next) {
            print_tree_stmt(i->stmt, tabs+4);
        }
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf("}\n");
        break;
    default:
        break;
    }
}

void free_expr(expr_t* expr) {
    if (expr == NULL) return;
    switch (expr->kind)
    {
    case expr_unop:
        free_expr(expr->unop.expr);
        break;
    case expr_binop:
        free_expr(expr->binop.left);
        free_expr(expr->binop.right);
        break;
    case expr_id:
        free((void*)expr->id);
    default:
        break;
    }
    free(expr);
}

void free_stmt_list(stmt_list_t* stmt_list__) {
    if (stmt_list__->stmt) {
        free_stmt(stmt_list__->stmt);
    }
    if (stmt_list__->next) {
        free_stmt_list(stmt_list__->next);
    }
    free(stmt_list__);
}

void free_stmt(stmt_t* stmt) {
    if (stmt == NULL) return;
    switch (stmt->kind)
    {
    case stmt_expr:
        free_expr(stmt->expr);
        break;
    case stmt_if:
        free_expr(stmt->if_stmt.condition);
        free_stmt(stmt->if_stmt.stmt);
        free_stmt(stmt->if_stmt.elseStmt);
        break;
    case stmt_while:
        free_expr(stmt->while_stmt.condition);
        free_stmt(stmt->while_stmt.stmt);
        break;
    case stmt_list:
        free_stmt_list(stmt->stmt_list);
        break;
    default:
        break;
    }
    free(stmt);
}

CSObject* execExpr(expr_t* expr) {
    switch (expr->kind)
    {
    case expr_binop:
        {
            CSObject* left = execExpr(expr->binop.left);
            CSObject* right = execExpr(expr->binop.right);
            switch (expr->binop.op)
            {
            case tok_add:
                return add(left, right);
                break;
            case tok_mul:
                return mul(left, right);
                break;
            case tok_sub:
                return sub(left, right);
                break;
            case tok_div:
                return _div(left, right);
                break;
            default:
                return NULL;
                break;
            }
        }
        break;
    case expr_unop:
        {
            CSObject* o = execExpr(expr->unop.expr);
            switch (expr->unop.op)
            {
            case tok_sub:
                return neg(o);
            default:
                return NULL;
                break;
            }
        }
    case expr_num:
        return createDoubleObject(expr->number);
    default:
        break;
    }
    return NULL;
}

void print(CSObject* object) {
    CSObject* _str = str(object);
    printf("%s ", getCStr(_str));
    decref(_str);
}
