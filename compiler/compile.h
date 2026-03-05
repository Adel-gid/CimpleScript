#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "../parser/parser.h"

typedef struct _class class_t;

typedef struct location {
    int kind;
    union {
        uint64_t imm;
        int64_t stackOff;
        uint64_t regNo;
        const char* globalName;
    };
} location_t;

enum LocationType {
    loc_reg,
    loc_stack,
    loc_imm,
    loc_global
};

typedef struct object {
    class_t* _class;
} object_t;



typedef object_t*(*PFNCSCAdd)(object_t*, object_t*);
typedef object_t*(*PFNCSCSub)(char* buff, int buffsize, object_t*, object_t*);
typedef struct CSCInterface {
    PFNCSCAdd add;
    PFNCSCSub sub;
} CSCInterface;

typedef struct _class {
    const char* name;
    uint64_t objectSize;
    CSCInterface* interface;
} class_t;

typedef struct CSVar {
    const char* name;
    object_t* object;
    struct CSVar* next;
} CSVar;

typedef struct CSScope {
    CSVar* vars;
    struct CSScope* parent;
} CSScope;

object_t* createObject(class_t* _class, location_t loc);
void freeObject(object_t* obj);

object_t* scopeGet(CSScope* currentScope, const char* name);
object_t* scopeCreate(CSScope* currentScope, const char* name, const char* className, location_t loc);
object_t* scopeSet(CSScope* currentScope, const char* name, object_t* object);
void scopeClean(CSScope* scope);

class_t* getInt32Class();

class_t* getClassByName(const char* name);
void registerClass(class_t* _class);

location_t getArgLoc(int argNo);

object_t* add(ir_t* ir, object_t* left, object_t* right);

typedef struct constant_pool_node {
    int kind;
    union {
        uint64_t _int;
        double   _float;
        const char* string;
    };
    struct constant_pool* next;
} constant_pool_node_t;

typedef struct constant_pool {
    constant_pool_node_t* head;
    constant_pool_node_t* tail;
} constant_pool_t;

enum ConstantKind {
    const_str,
    const_int,
    const_double
};
int getConstant(constant_pool_t* pool, int kind, ...);

void appendConstant(constant_pool_t* head, int kind, ...);
void destroyConstantPool(constant_pool_t* pool);

typedef struct ir_operand {
    object_t* left;
    object_t* right;
    object_t* dest;
    int op;
} ir_operand_t;

typedef struct ir_node {
    int kind;
    union {
        ir_operand_t operand;
        const char*  label;
    };
    struct ir* next;
} ir_node_t;

typedef struct ir {
    ir_node_t* head;
    ir_node_t* tail;
} ir_t;

enum irKind {
    ir_label,
    ir_jeq,
    ir_jlt,
    ir_jle,
    ir_jge,
    ir_jgt,
    ir_jmp,
    ir_cmp,
    ir_add,
    ir_sub,
    ir_mul,
    ir_div,
    ir_mod,
    ir_inc,
    ir_dec,
    ir_mov,
    ir_stackAlloc,
    ir_call,
    ir_push,
    ir_var,
    ir_return,
    ir_pusha
};

void emit(ir_t* ir, int kind, ...);

void print_ir(ir_t* ir);

void codegen(ir_t* ir, top_level_stmt_t* stmt);

void output(FILE* file, ir_t* ir);

#endif