#include "compile.h"
#include <stdio.h>

const char* registers[] = {
    "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
    "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};

class_t int32Class;

// Вспомогательная функция для получения строкового представления операнда
void getOperand(char* out, int outsize, object_t* obj) {
    switch (obj->loc.kind) {
        case loc_imm:
            snprintf(out, outsize, "%llu", obj->loc.imm);
            break;
        case loc_reg:
            snprintf(out, outsize, "%s", registers[obj->loc.regNo]);
            break;
        case loc_stack:
            snprintf(out, outsize, "[ebp%+lld]", obj->loc.stackOff);
            break;
        case loc_global:
            snprintf(out, outsize, "[rel %s]", obj->loc.globalName);
            break;
    }
}

int int32Add(char* buff, int buffsize, object_t* l, object_t* r) {
    char opL[64], opR[64];
    getOperand(opL, 64, l);
    getOperand(opR, 64, r);

    int written = 0;
    
    // 1. Загружаем левый операнд в целевой регистр (например, eax)
    // Если он уже там, mov можно пропустить (базовая оптимизация)
    if (l->loc.kind != loc_reg || l->loc.regNo != 0) {
        written += snprintf(buff + written, buffsize - written, 
                            "    mov eax, %s\n", opL);
    }
    
    // 2. Выполняем сложение с правым операндом
    written += snprintf(buff + written, buffsize - written, 
                        "    add eax, %s\n", opR);
    
    return written;
}

int int32Sub(char* buff, int buffsize, object_t* l, object_t* r) {
    char opL[64], opR[64];
    getOperand(opL, 64, l);
    getOperand(opR, 64, r);

    int written = 0;
    
    // 1. Загружаем левый операнд в целевой регистр (например, eax)
    // Если он уже там, mov можно пропустить (базовая оптимизация)
    if (l->loc.kind != loc_reg || l->loc.regNo != 0) {
        written += snprintf(buff + written, buffsize - written, 
                            "    mov eax, %s\n", opL);
    }
    
    // 2. Выполняем сложение с правым операндом
    written += snprintf(buff + written, buffsize - written, 
                        "    sub eax, %s\n", opR);
    
    return written;
}



CSCInterface int32Inteface = {
    .add = int32Add,
    .sub = int32Sub,
};

class_t int32Class = {
    .name = "int32",
    .interface = &int32Inteface,
    .objectSize = 4
};

class_t* getInt32Class() {
    return &int32Class;
}
