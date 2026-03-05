#include "compile.h"
#include <stdio.h>

void output(FILE* file, ir_t* ir) {
    for (ir_node_t* i = ir->head; i; i = i->next) {
        switch (i->kind)
        {
        case ir_label:
            fprintf(file, "%s:\n", i->label);
            break;
        case ir_jmp:
            fprintf(file, "    jmp %s\n", i->label);
            break;
        case ir_cmp:
            
        default:
            break;
        }
    }
}