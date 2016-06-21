#include <stdio.h>
#include <stdlib.h>
#include "get_images.h"

//return halt?
int i_func_ex(int cycle,  FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, int C){
    int tmp;
    //num overflow error
    if(type_num == ADDI || type_num == LW || type_num == LH || type_num == LHU || type_num == LB ||
            type_num == LBU || type_num == SW || type_num == SH || type_num == SB){
        tmp = reg[rs] + C;
        if(reg[rs]>=0 && C>=0 && tmp < 0){
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
        }else if(reg[rs]<0 && C<0 && tmp >= 0){
            fprintf(err_file, "In cycle %d: Number Overflow\n", cycle);
        }
    }
    return 0;
}

int i_func_mem(int cycle,  FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, int C){
    int return_num = 0;
    int tmp;
    //memory address overflow error
    if(type_num == LW || type_num == SW || type_num == SH || type_num == LHU || type_num == LH || type_num == SB
            || type_num == LBU  || type_num == LB){
        int temp;
        if(type_num == LW || type_num == SW) temp = 3;
        else if(type_num == SH || type_num == LHU || type_num == LH) temp = 1;
        else if(type_num == SB || type_num == LBU  || type_num == LB) temp = 0;
        if(reg[rs]+C+temp >= 1024 || reg[rs]+C < 0 || reg[rs]+C+temp < 0 || reg[rs]+C >= 1024){
            fprintf(err_file, "In cycle %d: Address Overflow\n", cycle);
            printf("In cycle %d: Address Overflow, reg[rs]+C = %08x\n", cycle, reg[rs] + C);
            return_num = 1;
        }
    }
    //misaligned error
    tmp = reg[rs]+C;
    if(type_num == LW || type_num == SW){
        if(tmp%4 != 0){
            fprintf(err_file, "In cycle %d: Misalignment Error\n", cycle);
            return_num = 1;
        }
    }else if(type_num == SH || type_num == LHU || type_num == LH){
        if(tmp%2 != 0){
            fprintf(err_file, "In cycle %d: Misalignment Error\n", cycle);
            return_num = 1;
        }
    }

    return return_num;
}
int i_func_test_0(int cycle,  FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, int C){


    int tmp;
    unsigned int unstmp;

    //write $0 error
    if(type_num != SW && type_num != SH && type_num != SB && type_num != BEQ && type_num != BNE){
        if(rt == 0){
            fprintf(err_file, "In cycle %d: Write $0 Error\n", cycle);
            return 0;
        }
    }
}
int i_func_wb(int cycle,  FILE* err_file, int *PC, int *reg, int *mem, int type_num, unsigned int rs, unsigned int rt, int C){


    int tmp;
    unsigned int unstmp;

    //write $0 error
    if(type_num != SW && type_num != SH && type_num != SB && type_num != BEQ && type_num != BNE){
        if(rt == 0){
            fprintf(err_file, "In cycle %d: Write $0 Error\n", cycle);
            return 0;
        }
    }
    switch(type_num){
        case ADDI:
            reg[rt] = reg[rs] + C;
            break;
        case LW:
            reg[rt] = (mem[reg[rs] + C]<<24) + (mem[reg[rs] + C+1]<<16) + (mem[reg[rs] + C+2]<<8) + (mem[reg[rs] + C+3]);
            break;
        case LH:
            tmp = (mem[reg[rs] + C]<<8) + (mem[reg[rs] + C+1]);
            tmp = tmp << 16;
            reg[rt] = tmp >> 16;
            break;
        case LHU:
            tmp = (mem[reg[rs] + C]<<8) + (mem[reg[rs] + C+1]);
            unstmp = tmp << 16;
            reg[rt] = unstmp >> 16;
            break;
        case LB:
            tmp = mem[reg[rs] + C];
            tmp = tmp << 24;
            reg[rt] = tmp >> 24;
            break;
        case LBU:
            tmp = mem[reg[rs] + C];
            unstmp = tmp << 24;
            reg[rt] = unstmp >> 24;
            break;
        case SW:
            mem[reg[rs] + C] = reg[rt]>>24;
            tmp = reg[rt] & 0x00ff0000;
            mem[reg[rs] + C+1] = tmp>>16;
            tmp = reg[rt] & 0x0000ff00;
            mem[reg[rs] + C+2] = tmp>>8;
            tmp = reg[rt] & 0x000000ff;
            mem[reg[rs] + C+3] = tmp;
            break;
        case SH:
            tmp = reg[rt] & 0x0000ff00;
            mem[reg[rs] + C] = tmp>>8;
            tmp = reg[rt] & 0x000000ff;
            mem[reg[rs] + C+1] = tmp;
            break;
        case SB:
            mem[reg[rs] + C] = reg[rt] & 0x000000ff;
            break;
        case LUI:
            reg[rt] = (C << 16);
            break;
        case ANDI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = reg[rs] & unstmp;
            break;
        case ORI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = reg[rs] | unstmp;
            break;
        case NORI:
            unstmp = (C<<16);
            unstmp = (unstmp>>16);
            reg[rt] = ~(reg[rs] | unstmp);
            break;
        case SLTI:
            reg[rt] = (reg[rs] < C)? 1:0;
            break;
        case BEQ:
            break;
        case BNE:
            break;
        default:
            break;
    }
    return 0;
}

