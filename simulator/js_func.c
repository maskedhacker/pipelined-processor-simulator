#include <stdio.h>
#include <stdlib.h>
#include "get_images.h"
//return halt?
int js_func_change_PC(int cycle, FILE* err_file, int *PC, int *reg, int *mem, int type_num, int C){
    switch(type_num){
        unsigned int tmp1;
        int tmp2;
        //*PC += 4;

        case J:
            tmp1 = C<<6;
            tmp1 = tmp1>>4;
            tmp2 = (*PC)&0xf0000000;
            *PC = tmp1+tmp2-4;
            break;
        case JAL:
            tmp1 = C<<6;
            tmp1 = tmp1>>4;
            tmp2 = (*PC)&0xf0000000;
            *PC = tmp1+tmp2-4;
            break;
        case HALT:
            break;
        default:
            break;
    }
    return 0;
}
