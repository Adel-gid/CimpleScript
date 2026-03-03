#include "../parser/parser.h"
#include "compile.h"
#include <stdio.h>

constant_pool_t pool = { 0 };

int localLabelCount = 1;

void codegenExpr(ir_t* ir, expr_t* expr) {
    if (expr == NULL) return;
    switch (expr->kind)
    {
    case expr_id:
        emit(ir, ir_var, expr->id);
        break;
    case expr_num:
        {
            int n = getConstant(&pool, const_double, expr->number);
            emit(ir, ir_push, n);
        }
        break;
    case expr_str:
        {
            int n = getConstant(&pool, const_str, expr->str);
            emit(ir, ir_push, n);
        }
        break;
    case expr_list:
        for (expr_list_t* i = expr->expr_list; i; i = i->next) {
            codegenExpr(ir, i->expr);
        }
        break;
    case expr_unop:
        {
            codegenExpr(ir, expr->unop.expr);
            switch (expr->unop.op)
            {
            case tok_add:
                emit(ir, ir_inc);
                break;
            case tok_sub:
                emit(ir, ir_dec);
            default:
                break;
            }
            break;
        }
    case expr_binop:
        {
            if (expr->binop.op == tok_colon) {
                emit(ir, ir_stackAlloc, getClassByName(expr->binop.right->id)->objectSize);
                emit(ir, ir_var, expr->binop.left->id);
                break;
            }
            if (expr->binop.op == tok_lpar) {
                codegenExpr(ir, expr->binop.right);
                codegenExpr(ir, expr->binop.left);
                emit(ir, ir_call);
                break;
            }
            codegenExpr(ir, expr->binop.left);
            codegenExpr(ir, expr->binop.right);
            switch (expr->binop.op)
            {
            case tok_add:
                emit(ir, ir_add);
                emit(ir, ir_pusha);
                break;
            case tok_sub:
                emit(ir, ir_sub);
                emit(ir, ir_pusha);
                break;
            case tok_mul:
                emit(ir, ir_mul);
                emit(ir, ir_pusha);
                break;
            case tok_assign:
                emit(ir, ir_mov);
                emit(ir, ir_pusha);
                break;
            case tok_lt:
            case tok_gt:
            case tok_ge:
            case tok_le:
            case tok_eq:
            {
                emit(ir, ir_cmp);
                char buff[64];
                sprintf(buff, ".l%d", localLabelCount);
                switch (expr->binop.op)
                {
                case tok_lt:
                    emit(ir, ir_jlt, buff);
                    break;
                case tok_gt:
                    emit(ir, ir_jgt, buff);
                    break;
                case tok_le:
                    emit(ir, ir_jle, buff);
                    break;
                case tok_ge:
                    emit(ir, ir_jge, buff);
                    break;
                case tok_eq:
                    emit(ir, ir_jeq, buff);
                default:
                    break;
                }
                sprintf(buff, ".l%d", localLabelCount+1);
                emit(ir, ir_jmp, buff);
            }
            default:
                break;
            }
            break;
        }
    default:
        break;
    }
}


void codegenStmt(ir_t* ir, stmt_t* stmt) {
    if (stmt == NULL) return;
    switch (stmt->kind)
    {
    case stmt_expr:
        {
            codegenExpr(ir, stmt->expr);
        }
        break;
    case stmt_if:
        {
            codegenExpr(ir, stmt->if_stmt.condition);
            char buff[64];
            sprintf(buff, ".l%d", localLabelCount);
            emit(ir, ir_label, buff);
            codegenStmt(ir, stmt->if_stmt.stmt);
            sprintf(buff, ".l%d", localLabelCount+1);
            emit(ir, ir_label, buff);
            if (stmt->if_stmt.elseStmt) {
                codegenStmt(ir, stmt->if_stmt.elseStmt);
            }
            localLabelCount += 2;
            break;
        }
    case stmt_while:
        {
            char buff[64];
            sprintf(buff, ".l%d", localLabelCount);
            emit(ir, ir_label, buff);
            localLabelCount += 1;
            codegenExpr(ir, stmt->while_stmt.condition);
            localLabelCount -= 1;
            sprintf(buff, ".l%d", localLabelCount+1);
            emit(ir, ir_label, buff);
            codegenStmt(ir, stmt->while_stmt.stmt);
            sprintf(buff, ".l%d", localLabelCount);
            emit(ir, ir_jmp, buff);
            sprintf(buff, ".l%d", localLabelCount+2);
            emit(ir, ir_label, buff);
            localLabelCount += 3;
            break;
        }
    case stmt_return:
        {
            codegenExpr(ir, stmt->_return_stmt);
            emit(ir, ir_jmp, ".ret");
            break;
        }
    case stmt_list:
        for (stmt_list_t* i = stmt->stmt_list; i; i = i->next) {
            codegenStmt(ir, i->stmt);
        }
    default:
        break;
    }
}

void codegen(ir_t* ir, top_level_stmt_t* stmt) {
    if (stmt == NULL) return;
    switch (stmt->kind)
    {
    case top_level_expr:
        codegenExpr(ir, stmt->expr);
        break;
    case top_level_list:
        for (top_level_stmt_list_t* i = stmt->list; i; i = i->next) {
            codegen(ir, i->_stmt);
        }
        break;
    case top_level_func:
        {
            emit(ir, ir_label, stmt->func_def.definition->binop.left->id);
            codegenStmt(ir, stmt->func_def.body);
            emit(ir, ir_label, ".ret");
            emit(ir, ir_return);
            localLabelCount = 1;
            return;
        }
    default:
        break;
    }
}