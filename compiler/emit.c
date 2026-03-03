#include "compile.h"
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void emit(ir_t* ir, int kind, ...) {
    va_list va;
    va_start(va, kind);
    ir_node_t* newNode = malloc(sizeof(ir_node_t));
    newNode->kind = kind;
    newNode->next = NULL;
    switch (newNode->kind)
    {
    case ir_label:
    case ir_jeq:
    case ir_jge:
    case ir_jgt:
    case ir_jle:
    case ir_jlt:
    case ir_jmp:
    case ir_var:
        newNode->label = strdup(va_arg(va, const char*));
        break;
    case ir_push:
    case ir_stackAlloc:
        newNode->imm = va_arg(va, uint64_t);
        break;
    default:
        break;
    }
    if (ir->tail) {
        ir->tail->next = newNode;
    }
    ir->tail = newNode;
    if (ir->head == NULL) {
        ir->head = ir->tail;
    }
    va_end(va);
}

void appendConstantv(constant_pool_t* head, int kind, va_list va) {
    constant_pool_node_t* newNode = malloc(sizeof(constant_pool_t));
    newNode->kind = kind;
    newNode->next = NULL;
    switch (newNode->kind)
    {
    case const_str:
        newNode->string = strdup(va_arg(va, const char*));
        break;
    case const_double:
        newNode->_float = va_arg(va, double);
        break;
    case const_int:
        newNode->_int = va_arg(va, uint64_t);
    default:
        break;
    }
        if (head->tail) {
        head->tail->next = newNode;
    }
    head->tail = newNode;
    if (head->head == NULL) {
        head->head = head->tail;
    }
}

void appendConstant(constant_pool_t* head, int kind, ...) {
    va_list va;
    va_start(va, kind);
    appendConstantv(head, kind, va);
    va_end(va);
}

int getConstant(constant_pool_t* pool, int kind, ...) {
    va_list va;
    va_start(va, kind);
    // Сохраняем значение аргумента
    const char* s = NULL;
    uint64_t i = 0;
    double d = 0;
    if (kind == const_str) s = va_arg(va, const char*);
    else if (kind == const_int) i = va_arg(va, uint64_t);
    else if (kind == const_double) d = va_arg(va, double);
    va_end(va);
    int n = 0;
    for (constant_pool_node_t* i = pool->head; i; i = i->next, ++n) {
        if (i->kind != kind) continue;
        if (kind == const_str) {
            if (strcmp(i->string, s) == 0) {
                return n;
            }
        }
        if (kind == const_int) {
            if (i->_int == i) return n;
        }
        if (kind == const_double) {
            if (i->_float == d) return n;
        }
    }
    appendConstantv(pool, kind, va);
    return n;
}

void destroyConstantPool(constant_pool_t* pool) {
    for (constant_pool_node_t* i = pool->head; i;) {
        if (i->kind == const_str) free(i->string);
        constant_pool_t* prev = i;
        i = i->next;
        free(prev);
    }
}

const char* ir_opcode_to_string(int _ir) {
    static const char* strings[] = {
        "label",
        "jeq",
        "jlt",
        "jle",
        "jge",
        "jgt",
        "jmp",
        "cmp",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "inc",
        "dec",
        "mov",
        "stackAlloc",
        "call",
        "push",
        "var",
        "return",
        "pusha"
    };
    return strings[_ir];
}

void print_ir(ir_t* ir) {
    for (ir_node_t* i = ir->head; i; i = i->next) {

        switch (i->kind)
        {
        case ir_label:
            printf("%s:\n", i->label);
            break;
        case ir_jeq:
        case ir_jge:
        case ir_jgt:
        case ir_jle:
        case ir_jlt:
        case ir_jmp:
        case ir_var:
            printf("    %s %s\n", ir_opcode_to_string(i->kind), i->label);
            break;
        case ir_push:
        case ir_stackAlloc:
            printf("    %s %llu\n", ir_opcode_to_string(i->kind), i->imm);
            break;
        default:
            printf("    %s\n", ir_opcode_to_string(i->kind));
            break;
        }

    }
}
