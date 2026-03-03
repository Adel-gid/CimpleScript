
#include "../parser/parser.h"
#include <stdio.h>

typedef struct FunctionData {
    int argc;
    stmt_t* stmt;
    CSScope scope;
    CSObject* result;
} FunctionData;

extern const char* input;

CSObject* execExpr(CSScope* scope, expr_t* expr) {
    if (expr == NULL) return NULL;
    switch (expr->kind)
    {
    case expr_binop:
        {
            if (expr->binop.op == tok_colon) {
                if (expr->binop.left->kind != expr_id ||
                    expr->binop.right->kind != expr_id) 
                    return createExceptionObject("TypeError", NULL, 0, 0, "Expected identifer");
                const char* name = expr->binop.left->id;
                const char* className = expr->binop.right->id;
                CSObject* result = scopeCreate(scope, name, className);
                return result;
            }
            if (expr->binop.op == tok_lpar) {
                CSObject* left = execExpr(scope, expr->binop.left);
                int argc = 0;
                expr_t* args = expr->binop.right;
                for (expr_list_t* i = args->expr_list; i; i = i->next, ++argc);
                CSObject** argv = malloc(sizeof(CSObject*)*argc);
                argc = 0;
                for (expr_list_t* i = args->expr_list; i; i = i->next, ++argc) {
                    argv[argc] = execExpr(scope, i->expr);
                }
                CSObject* result = call(left, argc, argv);
                for (int i = 0; i < argc; ++i) {
                    decref(argv[i]);
                }
                free(argv);
                return result;
            }
            CSObject* left = execExpr(scope, expr->binop.left);
            CSObject* right = execExpr(scope, expr->binop.right);
            CSObject* result = NULL;
            switch (expr->binop.op)
            {
            case tok_add:
                result =  add(left, right);
                break;
            case tok_mul:
                result =  mul(left, right);
                break;
            case tok_sub:
                result =  sub(left, right);
                break;
            case tok_div:
                result =  _div(left, right);
                break;
            case tok_lt:
                result = lt(left, right);
                break;
            case tok_gt:
                result = gt(left, right);
                break;
            case tok_le:
                result = le(left, right);
                break;
            case tok_ge:
                result = ge(left, right);
                break;
            case tok_eq:
                result = eq(left, right);
                break;
            case tok_assign:
                {
                    CSObject* result = set(left, right);
                    decref(right);
                    return result;
                }
                break;
            default:
                return NULL;
                break;
            }
            decref(left);
            decref(right);
            return result;
        }
        break;
    case expr_id:
        {
            CSObject* object = scopeGet(scope, expr->id);
            if (object == NULL) {
                return createExceptionObject(
                    "VariableName", NULL, 0, 0, "Don't find var"
                );
            }
            return object;
        }
    case expr_unop:
        {
            CSObject* o = execExpr(scope, expr->unop.expr);
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
    case expr_str:
        return createStringObject(expr->str);
    case expr_error:
        return createExceptionObject(
            "SyntaxError",
            input, expr->_error.line,
            expr->_error.column, expr->_error.description
        );
    default:
        break;
    }
    return NULL;
}

CSObject* execStmt(CSScope* scope, stmt_t* stmt) {
    if (stmt == NULL) return NULL;
    switch (stmt->kind)
    {
    case stmt_expr:
    {
        CSObject* object = execExpr(scope, stmt->expr);
        if (object->__class__ == getExceptionClass()) {
            return object;
        }
        return NULL;
    }
    case stmt_while:
        {
            CSObject* result = NULL;
            while (1) {
                CSObject* _cond = execExpr(scope, stmt->while_stmt.condition);
                if (_cond->__class__ == getExceptionClass()) {
                    return _cond;
                }
                int _boolean = toCBool(_cond);
                decref(_cond);
                if (!_boolean) break;
                result = execStmt(scope, stmt->while_stmt.stmt);
                if (result->__class__ == getExceptionClass()) {
                    return result;
                }
                if (result) break;
            }
            return result;
        } 
    case stmt_if:
        {
            CSObject* _cond = execExpr(scope, stmt->if_stmt.condition);
            if (_cond->__class__ == getExceptionClass()) {
                    return _cond;
            }
            int _boolean = toCBool(_cond);
            decref(_cond);
            CSObject* result = NULL;
            if (_boolean) {
                result = execStmt(scope, stmt->if_stmt.stmt);
            }
            else {
                if (stmt->if_stmt.elseStmt)
                    result = execStmt(scope, stmt->if_stmt.elseStmt);
            }
            return result;
        }
        // Внутри execStmt для stmt_list
    case stmt_list: {
        CSScope newScope = { .vars = NULL, .parent = scope };
        stmt_list_t* current = stmt->stmt_list;
        CSObject* result = NULL;

        while (current) {
            result = execStmt(&newScope, current->stmt);
            if (result != NULL) { 
                // Если получили результат, значит сработал return.
                // Прекращаем выполнение списка инструкций и пробрасываем результат вверх.
                break; 
            }
            current = current->next;
        }
        scopeClean(&newScope);
        return result; // Возвращаем результат return или NULL
    }

    case stmt_return: {
        // Вычисляем значение и возвращаем его. 
        // Больше никаких глобальных флагов!
        return execExpr(scope, stmt->_return_stmt);
    }
    default:
        break;
    }
    return NULL;
}

void execTopLevelStmt(CSScope* scope, top_level_stmt_t* top_level) {
    if (top_level == NULL) return;
    switch (top_level->kind)
    {
    case top_level_func:
        {
            expr_t* expr = top_level->func_def.definition;
            expr_t* name = expr->binop.left;
            expr_t* argv = expr->binop.right;
            CSObject* newFunction = scopeCreate(scope, name->id, "function");
            FunctionData* data = newFunction->objectData;
            data->scope.parent = scope;
            data->scope.vars = NULL;
            data->stmt = top_level->func_def.body;
            data->argc = 0;
            for (expr_list_t* i = argv->expr_list; i; i = i->next, ++data->argc) {
                execExpr(&data->scope, i->expr);
            }
        }
        break;
    case top_level_expr:
        {
            execExpr(scope, top_level->expr);
        }
        break;
    case top_level_list:
        {
            for (top_level_stmt_list_t* i = top_level->list; i; i = i->next) {
                execTopLevelStmt(scope, i->_stmt);
            }
        }
        break;
    default:
        break;
    }
}
