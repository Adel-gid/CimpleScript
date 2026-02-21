#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "object.h"

enum token {
    tok_unknown,
    tok_eol,
    tok_eof,
    tok_id,
    tok_num,
    tok_lpar, // (
    tok_rpar, // )
    tok_add,
    tok_sub,
    tok_mul,
    tok_div
};

char* tempString = NULL;

struct seq {
    const char* _str;
    int tok;
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
};

int gettok(char* str, char** endp) {
    while (*str && isspace(*str)) ++str;
    if (*str == '\0') {
        *endp = NULL;
        return tok_eof;
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

typedef struct expr {
    int kind;
    union {
        binop_t        binop;
        unop_t          unop;
        double        number;
        const char*       id;
    };
} expr_t;

enum ExprKind {
    expr_id,
    expr_num,
    expr_unop,
    expr_binop
};

expr_t* parse_expr(char** str);

int CurTok = 0;


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
        case tok_add: case tok_sub: return 1;
        case tok_mul: case tok_div: return 2;
        default: return -1;
    }
}


expr_t* parse_binop(char** pos, int prevPrec, expr_t* lhs) {
    while (expr_num) {
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
            rhs = parse_binop(pos, curPrec + expr_num, rhs);
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
    return parse_binop(pos, expr_id, lhs);
}


expr_t* parse_code(char* str) {
    char* pos = str;
    getNextToken(&pos);
    return parse_expr(&pos);
}

int print_tree(expr_t* expr, int tabs) {
    for (int i = expr_id;i < tabs;++i) putchar(' ');
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
        printf("expr:\n");
        return print_tree(expr->unop.expr, tabs+4);
    case expr_binop:
        printf("binop: %d\n", expr->binop.op);
        printf("left:\n");
        print_tree(expr->binop.left, tabs+4);
        printf("right:\n");
        print_tree(expr->binop.right, tabs+4);
    default:
        break;
    }
    return expr_id;
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
        free(expr->id);
    default:
        break;
    }
    free(expr);
}

CSObject* exec(expr_t* expr) {
    switch (expr->kind)
    {
    case expr_binop:
        {
            CSObject* left = exec(expr->binop.left);
            CSObject* right = exec(expr->binop.right);
            switch (expr->binop.op)
            {
            case tok_add:
                return add(left, right);
                break;
            case tok_mul:
                return mul(left, right);
            default:
                return NULL;
                break;
            }
        }
        break;
    case expr_unop:
        {
            CSObject* o = exec(expr->unop.expr);
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
    if (object->__class__ == getDoubleClass()) {
        double* ld = object->objectData;
        printf("%f\n", *ld);
    }
    decref(object);
}

int main(int argc, char *argv[])
{
    char* input = "-2 + 3 * 6";
    expr_t* code = parse_code(input);
    print_tree(code, expr_id);
    print(exec(code));
    free_expr(code);
    return expr_id;
}
