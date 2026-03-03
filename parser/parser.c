#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "../interpreter/object.h"
#include "parser.h"


char* tempString = NULL;


//base structure for sequences
//it's needed to use seq_table
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
    {"else", tok_else},
    {"return", tok_return}
};

struct seq seqs[] = {
    {"\n", tok_eol},
    {"+", tok_add},
    {"-", tok_sub},
    {"/", tok_div},
    {"*", tok_mul},
    {"(", tok_lpar},
    {")", tok_rpar},
    {"->", tok_arrow}, //->
    {".", tok_point}, //.
    {"<=", tok_le}, //<=
    {">=", tok_ge}, //>=
    {"==", tok_eq}, //==
    {"=", tok_assign}, //=
    {"<", tok_lt}, //<
    {">", tok_gt}, //>
    {"{", tok_flpar},
    {"}", tok_frpar},
    {",", tok_comma},
    {":", tok_colon},
};

const char* input = NULL;

int line = 1;
int column = 1;
const char* description = NULL;

int gettok(char* str, char** endp) {
    if (input == NULL) {
        input = str;
    }
    while (*str && (*str == ' ' || *str == '\t' || *str == '\r')) {
        ++str;
        ++column;
    }
    if (*str == '\0') {
        *endp = NULL;
        return tok_eof;
    }
    for (struct seq* i = keywords; i < keywords + sizeof(keywords)/sizeof(*keywords); ++i) {
        int len = strlen(i->_str);
        if (strncmp(str, i->_str, len) == 0) {
            str += len;
            column += len;
            *endp = str;
            return i->tok;
        }
    }
    if (isdigit(*str)) {
        int n = 0;
        for (; isdigit(*str) && *str; ++n, ++str);
        tempString = malloc(n+1);
        memcpy(tempString, str-n, n);
        column += n;
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
        column += n;
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
        column += n+1;
        ++str;
        *endp = str;
        return tok_str;
    }
    if (*str == '/') {
        ++str;
        if (*str == '/') {
            while (*str && *str != '\n') {++str; ++column;}
            return gettok(str, endp);
        }
        --str; //it's div operator
    }
    for (struct seq* i = seqs; i < seqs + sizeof(seqs)/sizeof(*seqs); ++i) {
        int len = strlen(i->_str);
        if (strncmp(str, i->_str, len) == 0) {
            if (*str == '\n') {
                ++line;
                column = 0;
            }
            str += len;
            column += len;
            *endp = str;
            return i->tok;
        }
    }
    description = "Unknown token";
    return tok_unknown;
}



expr_t* parse_expr(char** str);

int CurTok = 0;

void free_expr(expr_t* expr);

int getNextToken(char** pos) {
    CurTok = gettok(*pos, pos);
    return CurTok;
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
expr_t* parse_str(char** pos) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->kind = expr_str;
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

expr_t* expr_create_error(const char* description) {
    expr_t* expr = malloc(sizeof(expr_t));
    expr->_error.column = column;
    expr->_error.line = line;
    expr->_error.description = strdup(description);
    expr->kind = expr_error;
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
        case tok_str:
            expr = parse_str(pos);
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
        case tok_unknown:
            expr = expr_create_error(description);
            return expr;
        default:
            {
                char buff[64];
                sprintf(buff, "unknown error: %d", CurTok);
                expr = expr_create_error(buff);
                return expr;
            }

    }
    return expr;
}


int getTokPrec(int tok) {
    switch (tok) {
        case tok_assign:
            return 1;
        case tok_lt: case tok_gt: case tok_le: case tok_ge: case tok_eq: 
            return 5; // Приоритет ниже, чем у сложения
        case tok_add: case tok_sub: 
            return 10;
        case tok_mul: case tok_div: 
            return 20;
        case tok_colon:
            return 35;
        case tok_lpar:
            return 40;
        default: 
            return -1;
    }
}

expr_t* parse_binop(char** pos, int prevPrec, expr_t* lhs) {
    while (1) {
        int curPrec = getTokPrec(CurTok);
        if (curPrec < prevPrec) return lhs;

        int op = CurTok;
        expr_t* rhs = NULL;
        if (op == tok_lpar) {
            getNextToken(pos); // Пропускаем '('
            
            expr_list_t* head = NULL;
            expr_list_t* last = NULL;

            // Обработка списка аргументов
            if (CurTok != tok_rpar) { // Если сразу ')', то аргументов 0
                while (1) {
                    expr_t* arg = parse_expr(pos);
                    if (arg->kind == expr_error) return arg;

                    expr_list_t* node = malloc(sizeof(expr_list_t));
                    node->expr = arg;
                    node->next = NULL;

                    if (!head) head = node;
                    if (last) last->next = node;
                    last = node;

                    if (CurTok == tok_comma) {
                        getNextToken(pos); // Пропускаем запятую и идем на след. круг
                    } else if (CurTok == tok_rpar) {
                        break; // Конец списка аргументов
                    } else {
                        return expr_create_error("expected comma or rpar");
                    }
                }
            }
            getNextToken(pos); // Пропускаем ')'

            // Создаем узел для списка аргументов
            rhs = malloc(sizeof(expr_t));
            rhs->kind = expr_list;
            rhs->expr_list = head;
        } else {
            getNextToken(pos);
            rhs = parse_primary(pos);
        }
        if (rhs->kind == expr_error) return rhs;

        int nextPrec = getTokPrec(CurTok);
        if (curPrec < nextPrec) {
            rhs = parse_binop(pos, curPrec + 1, rhs);
            if (rhs->kind == expr_error) return rhs;
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
    if (lhs->kind == expr_error) return lhs;
    return parse_binop(pos, 0, lhs);
}

stmt_t* parse_stmt(char** input);

stmt_t* parse_expr_stmt(char** str) {
    stmt_t* newStmt = malloc(sizeof(stmt_t));
    newStmt->kind = stmt_expr;
    newStmt->expr = parse_expr(str);
    return newStmt;
}

stmt_t* parse_stmt_list(char** str) {
    getNextToken(str); // пропускаем '{'
    stmt_list_t* head = NULL;
    stmt_list_t* tail = NULL;

    while (CurTok != tok_frpar) {
        while (CurTok == tok_eol) getNextToken(str);
        if (CurTok == tok_frpar) break;

        stmt_t* stmt = parse_stmt(str);
        if (stmt->kind == stmt_expr && stmt->expr->kind == expr_error) return stmt;
        if (CurTok != tok_eol && CurTok != tok_eof && CurTok != tok_frpar) {
            expr_t* error = expr_create_error("expected eol");
            stmt_t* result = malloc(sizeof(stmt_t));
            result->kind = stmt_expr;
            result->expr = error;
            return result;
        }
        stmt_list_t* node = malloc(sizeof(stmt_list_t));
        node->stmt = stmt;
        node->next = NULL;

        if (!head) head = node;
        else tail->next = node;
        tail = node;
    }

    getNextToken(str); // пропускаем '}'

    stmt_t* result = malloc(sizeof(stmt_t));
    result->kind = stmt_list;
    result->stmt_list = head;
    return result;
}

stmt_t* parse_while_stmt(char** str) {
    getNextToken(str);
    expr_t* cond = parse_expr(str);
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

stmt_t* parse_return(char** str) {
    getNextToken(str);
    expr_t* _return = parse_expr(str);
    stmt_t* result = malloc(sizeof(stmt_t));
    result->kind = stmt_return;
    result->_return_stmt = _return;
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
    case tok_return:
        return parse_return(str);
    default:
        // {
        //     expr_t* error = expr_create_error("unknown statement");
        //     stmt_t* result = malloc(sizeof(stmt_t));
        //     result->expr = stmt_expr;
        //     result->expr = error;
        //     return result;
        // }
    }
    return parse_expr_stmt(str);
}

top_level_stmt_t* parse_function(char** pos) {
    getNextToken(pos); //eat 'function' keyword
    expr_t* def = parse_expr(pos);
    top_level_stmt_t* top_level = malloc(sizeof(top_level_stmt_t));
    if (def->kind == expr_error) {
        top_level->kind = top_level_expr;
        top_level->expr = def;
        return top_level;
    }
    stmt_t* body = parse_stmt(pos);
    if (body->kind == stmt_expr && body->expr->kind == expr_error) {
        top_level->kind = top_level_expr;
        top_level->expr = body->expr;
        return top_level;
    }
    top_level->kind = top_level_func;
    top_level->func_def.definition = def;
    top_level->func_def.body = body;
    return top_level;
}

top_level_stmt_t* parse_top_expr(char** pos) {
    expr_t* _expr = parse_expr(pos);
    top_level_stmt_t* top_level = malloc(sizeof(top_level_stmt_t));
    top_level->kind = top_level_expr;
    top_level->expr = _expr;
    return top_level;
}

top_level_stmt_t* parse_top_stmt(char** pos) {
    top_level_stmt_list_t* head = NULL;
    top_level_stmt_list_t* tail = NULL;
    while (1) {
        top_level_stmt_t* _stmt = NULL;
        while (CurTok == tok_eol) getNextToken(pos);
        if (CurTok == tok_eof) break;
        switch (CurTok)
        {
        case tok_function:
            _stmt = parse_function(pos);
            break;
        default:
            _stmt = parse_top_expr(pos);
            break;
        }
        if (_stmt->kind == top_level_expr && _stmt->expr->kind == expr_error) {
            return _stmt;
        }
        top_level_stmt_list_t* newList = malloc(sizeof(top_level_stmt_list_t));
        newList->_stmt = _stmt;
        newList->next = NULL;
        if (tail) {
            tail->next = newList;
        }
        tail = newList;
        if (head == NULL) {
            head = tail;
        }
    }
    top_level_stmt_t* result = malloc(sizeof(top_level_stmt_t));
    result->kind = top_level_list;
    result->list = head;
    return result;
}

top_level_stmt_t* parse_code(char* pos) {
    getNextToken(&pos);
    return parse_top_stmt(&pos);
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
    case expr_list:
        printf("(\n");
        for (expr_list_t* i = expr->expr_list; i; i = i->next) {
            print_tree_expr(i->expr, tabs+4);
        }
        for (int i = 0;i < tabs;++i) putchar(' ');
        printf(")\n");
        break;
    case expr_error:
        printf("%u:%u:error: %s\n", expr->_error.line, expr->_error.column,
            expr->_error.description);
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
            for (int i = 0;i < tabs;++i) putchar(' ');
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

int print_tree_top_stmt(top_level_stmt_t* top_stmt, int tabs) {
    for (int i = 0;i < tabs;++i) putchar(' ');
    if (top_stmt == NULL) printf("(null)\n");
    switch (top_stmt->kind)
    {
    case top_level_func:
        printf("function:\n");
        for (int i = 0;i < tabs;++i) putchar(' ');
        print_tree_expr(top_stmt->func_def.definition, tabs+4);
        for (int i = 0;i < tabs;++i) putchar(' ');
        print_tree_stmt(top_stmt->func_def.body, tabs+4);
        break;
    case top_level_expr:
        return print_tree_expr(top_stmt->expr, tabs+4);
    case top_level_list:
        for (top_level_stmt_list_t* i = top_stmt->list; i; i = i->next) {
            print_tree_top_stmt(i->_stmt, tabs+4);
        }
        for (int i = 0;i < tabs;++i) putchar(' ');
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
        break;
    case expr_str:
        free((void*)expr->id);
        break;
    case expr_list:
        for (expr_list_t* i = expr->expr_list; i; ) {
            expr_list_t* next = i->next;
            free_expr(i->expr);
            free(i);
            i = next;
        }
        break;
    case expr_error:
        free(expr->_error.description);
        break;
    default:
        break;
    }
    free(expr);
}

void free_stmt_list(stmt_list_t* stmt_list__) {
    if (!stmt_list__) return;
    if (stmt_list__->stmt) {
        free_stmt(stmt_list__->stmt);
    }
    if (stmt_list__->next) {
        free_stmt_list(stmt_list__->next);
    }
    free(stmt_list__);
}

void free_top_stmt_list(top_level_stmt_list_t* stmt_list__) {
    if (!stmt_list__) return;
    if (stmt_list__->_stmt) {
        free_top_stmt(stmt_list__->_stmt);
    }
    if (stmt_list__->next) {
        free_top_stmt_list(stmt_list__->next);
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

void free_top_stmt(top_level_stmt_t* top_stmt) {
    if (top_stmt == NULL) return;
    switch (top_stmt->kind)
    {
    case top_level_func:
        free_expr(top_stmt->func_def.definition);
        free_stmt(top_stmt->func_def.body);
        break;
    case top_level_expr:
        free_expr(top_stmt->expr);
        break;
    case top_level_list:
        free_top_stmt_list(top_stmt->list);
        break;
    default:
        break;
    }
    free(top_stmt);
}

