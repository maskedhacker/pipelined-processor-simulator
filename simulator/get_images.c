#include <stdio.h>
#include <stdlib.h>
#include "get_images.h"

int get_iimage(unsigned int* PC, INST** inst_arr){

    FILE* iimage = fopen("iimage.bin", "rb");
    if(iimage == 0){
        printf("Can't open iimage.bin\n");
        exit(-2);
    }
    int PC_val;
    //get PC
    int a,b,c,d;
    a = fgetc(iimage);
    b = fgetc(iimage);
    c = fgetc(iimage);
    d = fgetc(iimage);
    PC_val = (a<<24)+(b<<16)+(c<<8)+(d);
    *PC = PC_val;

    //get instructions number
    int ins_num;
    a = fgetc(iimage);
    b = fgetc(iimage);
    c = fgetc(iimage);
    d = fgetc(iimage);
    ins_num = (a<<24)+(b<<16)+(c<<8)+(d);
    //printf("ins_num = %d\n", ins_num);

    int i;
    int inp;
    unsigned int opcode;
    int tmp;
    unsigned int un_tmp;
    INST* ti;
    for(i = PC_val/4; i < ins_num+PC_val/4; i++){
        a = fgetc(iimage);
        b = fgetc(iimage);
        c = fgetc(iimage);
        d = fgetc(iimage);
        inp = (a<<24)+(b<<16)+(c<<8)+(d);
        inst_arr[i]->val = inp;
        opcode = (unsigned)inp >> 26;
        if(opcode==0x00){
            inst_arr[i]->type = R;
            tmp = inp<<26;
            inst_arr[i]->r_type = (unsigned)tmp>>26;
            inst_arr[i]->i_type = -1;
            inst_arr[i]->js_type = -1;
            tmp = inp<<6;
            inst_arr[i]->rs = (unsigned)tmp>>27;
            tmp = inp<<11;
            inst_arr[i]->rt = (unsigned)tmp>>27;
            tmp = inp<<16;
            inst_arr[i]->rd = (unsigned)tmp>>27;
            tmp = inp<<21;
            inst_arr[i]->capital_c = (signed)tmp>>27;
        }else if(opcode == 0x02 || opcode == 0x03 || opcode == 0x3F){
            inst_arr[i]->type = JS;
            inst_arr[i]->js_type = opcode;
            inst_arr[i]->i_type = -1;
            inst_arr[i]->r_type = -1;
            un_tmp = inp<<6;
            un_tmp = (unsigned)un_tmp>>6;
            inst_arr[i]->capital_c = (unsigned)un_tmp;
        }else{
            inst_arr[i]->type = I;
            inst_arr[i]->i_type = opcode;
            inst_arr[i]->r_type = -1;
            inst_arr[i]->js_type = -1;
            tmp = inp<<6;
            inst_arr[i]->rs = (unsigned)tmp>>27;
            tmp = inp<<11;
            inst_arr[i]->rt = (unsigned)tmp>>27;
            tmp = inp<<16;
            inst_arr[i]->capital_c = (signed)tmp>>16;
        }
        ti = inst_arr[i];
        if(ti->type == R && ti->r_type == SLL && ti->rt == 0 && ti->rd == 0 && ti->capital_c == 0){
            ti->type = NOP;
            ti->r_type = -1;
        }
        if(ti->r_type == JR || ti->i_type == SW || ti->i_type == SH || ti->i_type == SB || ti->i_type == BEQ || ti->i_type == BNE || ti->js_type == J || ti->js_type == HALT){
            ti->reg_write = 0;
        }else
            ti->reg_write = 1;
        if(ti->i_type == LW || ti->i_type == LH || ti->i_type == LHU || ti->i_type == LB || ti->i_type == LBU ){
            ti->load_use = 1;
        }else
            ti->load_use = 0;
        if(ti->r_type == JR){
            ti->is_jr = 1;
            ti->is_bench = 1;
        }else
            ti->is_jr = 0;
        if(ti->i_type == BEQ || ti->i_type == BNE){
            ti->is_bench = 1;
            ti->is_beq = 0;
            ti->is_bne = 0;
            if(ti->i_type == BEQ) ti->is_beq = 1;
            else ti->is_bne = 1;
        }else{
            if(ti->r_type != JR)ti->is_bench = 0;
            ti->is_beq = 0;
            ti->is_bne = 0;
        }
        if(ti->js_type == JAL || ti->js_type == J)
            ti->is_j = 1;
        else
            ti->is_j = 0;
    }
    return ins_num;
}


int get_dimage(int* initialized_reg, int* mem_arr){
    int a,b,c,d;
    int i;
    int d_num;

    FILE* dimage = fopen("dimage.bin", "rb");
    if(dimage == 0){
        printf("Can't open dimage.bin\n");
        exit(-3);
    }

    a = fgetc(dimage);
    b = fgetc(dimage);
    c = fgetc(dimage);
    d = fgetc(dimage);
    d_num = (a<<24)+(b<<16)+(c<<8)+(d);
    *initialized_reg = d_num;

    int dmemory_num;
    a = fgetc(dimage);
    b = fgetc(dimage);
    c = fgetc(dimage);
    d = fgetc(dimage);
    dmemory_num = (a<<24)+(b<<16)+(c<<8)+(d);

    //printf("memory_num:  %d\n", dmemory_num);

    for(i = 0; i < 4*dmemory_num && i < 1024; i++){
        a = fgetc(dimage);
        *(mem_arr+i) = a;
        //printf("i = %d, 0x%04x", i, a);
    }
    return dmemory_num;
}

int add_ab(int a, int b){
    unsigned long int sum;
    unsigned long int la;
    unsigned long int lb;
    la = a;
    lb = b;
    sum = la + lb;
    //printf("ADD: %09x = %08x + %08x\n", sum, la, lb);
    int ret = sum;
    return ret;
}
