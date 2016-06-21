#include <stdio.h>
#include <stdlib.h>
#include "get_images.h"
#define IIMAGE 1024
#define DIMAGE 1024
#define MEM 1024
unsigned int PC;
int need_forwarding = 0;
char tmp_c[10];
int tmp;
int reg[32];
int virtual_reg[32];
int prev_virtual_reg[32];
int prev_prev_virtual_reg[32];
int prev_prev_prev_virtual_reg[32];
int prev_prev_prev_prev_virtual_reg[32];
unsigned int virtual_PC;
unsigned int PC_for_branch;
int forwarding[4] = {0,0,0,0};
int mforwarding[4] = {0,0,0,0};
int bforwarding[4] = {0,0,0,0}; //forward?, (ex_haz = 0, mem_haz = 1), (rs = 0, rt = 1, rs&rt = 2), $?
int stall = 0;
int flush = 0;
int mem_wb_write;
int ex_mem_write;
int id_ex_write;
int id_ex_need_rs;
int id_ex_need_rt;
int if_id_need_rs;
int if_id_need_rt;
INST* inst_mem[IIMAGE];
int mem[DIMAGE];
INST* if_id;
INST* id_ex;
INST* ex_mem;
INST* mem_wb;
INST* cur_inst;
void show_reg(FILE* output, unsigned int cycle);//show cycle, reg[], and PC
void show_mem(FILE* output, unsigned int cycle);
void show_virtual_reg(FILE* output, unsigned int cycle);//show cycle, reg[], and PC
void check();

int main(){
    printf("start\n");
    //initalize reg
    unsigned int i;
    INST* nop_inst = (INST*)malloc(sizeof(INST));
    nop_inst->rs = 0;
    nop_inst->capital_c = 0;
    nop_inst->rd = 0;
    nop_inst->rt = 0;
    nop_inst->type = NOP;
    INST* cur;
    int k;

    for(i = 0; i < 32; i++){
        reg[i] = 0;
        virtual_reg[i] = 0;
        prev_virtual_reg[i] = 0;
        prev_prev_virtual_reg[i] = 0;
        prev_prev_prev_virtual_reg[i] = 0;
        prev_prev_prev_prev_virtual_reg[i] = 0;
    }
    for(i = 0; i < IIMAGE; i++) inst_mem[i] = (INST*)malloc(sizeof(INST));
    //initalize mem

    if_id = nop_inst;
    id_ex = nop_inst;
    ex_mem = nop_inst;
    mem_wb = nop_inst;


    get_iimage(&PC, inst_mem);
    get_dimage(&reg[29], mem);
    virtual_reg[29] = reg[29];
    virtual_PC = PC;
    //define snapshot
    FILE* snapshot =fopen("snapshot.rpt", "w");
    FILE* err_file = fopen("error_dump.rpt", "w");
    FILE* output = fopen("compare.txt", "w");
    FILE* reg_f = fopen("reg_f.txt", "w");
    FILE* mem_f = fopen("mem_f.txt", "w");

    i = 0;
    int halt = 0;
    int halt1 = 0;
    while(PC< 1024 && halt == 0){
        halt1 = 0;
//if:
        if(id_ex->r_type == JR){
            PC = virtual_reg[id_ex->rs];
        }

        cur_inst = inst_mem[PC/4];
        cur = cur_inst;
        show_reg(snapshot, i);
        if(flush == 1) {
            if(if_id->is_bench == 1)PC += 4*if_id->capital_c-4;
            else if(if_id->is_j == 1){
                js_func_change_PC(i+1, err_file, &PC, reg, mem, if_id->js_type, if_id->capital_c);
            }
        }
        if(cur_inst->js_type == HALT && if_id->js_type == HALT && id_ex->js_type == HALT && ex_mem->js_type == HALT && mem_wb->js_type == HALT) break;
        inst_to_str(tmp_c, cur);
        if((stall == 0 && flush == 0) || (cur->is_bench == 1)){
            fprintf(reg_f, "cycle %d: %s: %d, %d, %d, %d(%06x)\n", i+1, tmp_c, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c);
            fprintf(mem_f, "cycle %d: %s: %d, %d, %d, %d(%06x)\n", i+1, tmp_c, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c);
            fprintf(output, "cycle %d: %s: %d, %d, %d, %d(%06x), PC = %08x\n", i+1, tmp_c, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c, PC);
            if(cur->type == R){
                r_func_wb(i+1, output, &virtual_PC, virtual_reg, mem, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
            }else if(cur->type == I){
                tmp = i_func_mem(i+1, output, &virtual_PC, virtual_reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
                if(tmp == 0)i_func_wb(i+1, output, &virtual_PC, virtual_reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
                if(cur->is_bench == 1) PC_for_branch = PC;
            }else if(cur->type == JS){
                js_func_change_PC(i+1, output, &virtual_PC, virtual_reg, mem, cur->js_type, cur->capital_c);
                if(cur->js_type == JAL)virtual_reg[31] = PC+4;
            }

        }else{
            fprintf(reg_f, "STALL: cycle %d: %s: %d, %d, %d, %d(%06x)\n", i+1, tmp_c, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c);
            fprintf(output, "STALL: cycle %d: %s: %d, %d, %d, %d(%06x), PC = %08x\n", i+1, tmp_c, cur->rs, cur->rt, cur->rd, cur->capital_c, cur->capital_c, PC);

        }
        show_mem(mem_f, i+1);
        show_virtual_reg(reg_f, i+1);

//wb:
        cur = mem_wb;
        if(mem_wb->type == R) {
            if(mem_wb->r_type == JR);
            else {
                halt = r_func_wb(i+1, err_file, &PC, reg, mem, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
            }
        }
        else if(mem_wb->type == I) {
            if(cur->load_use){
                i_func_test_0(i+1, err_file, &PC, reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
                reg[cur->rt] = prev_prev_prev_prev_virtual_reg[cur->rt];
                fprintf(reg_f, "That's load_use\n");
            }
            else if(cur->i_type == SW || cur->i_type == SH || cur->i_type == SB);
            else halt = i_func_wb(i+1, err_file, &PC, reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);

        }else if(cur->type == JS){
            if(cur->js_type == JAL) {
                reg[31] = prev_prev_prev_prev_virtual_reg[31];
                fprintf(reg_f, "JAL!!\n");
            }
        }
        halt1 += halt;
        mem_wb = ex_mem;

//mem:
        cur = ex_mem;
        if(cur->type == I){
            if(cur->i_type == SW || cur->i_type == SH || cur->i_type == SB || cur->i_type == LW || cur->i_type == LH || cur->i_type == LHU || cur->i_type == LB || cur->i_type == LBU){
                if(need_forwarding) halt = i_func_mem(i+1, err_file, &PC, prev_prev_prev_virtual_reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
                else halt = i_func_mem(i+1, err_file, &PC, reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
            }
        }
        halt1 += halt;
        ex_mem = id_ex;
//ex:
        cur = id_ex;
        if(cur->is_beq == 1 && flush == 1 && prev_prev_virtual_reg[cur->rs] == prev_prev_virtual_reg[cur->rt]) {
            PC = PC_for_branch;
        }else if(cur->is_bne == 1 && flush == 1 && prev_prev_virtual_reg[cur->rs] != prev_prev_virtual_reg[cur->rt]){
            PC = PC_for_branch;
        }

        //if(cur->type == NOP) exit(1);
        if(cur->type == R){
            if(need_forwarding) halt = r_func_ex(i+1, err_file, &PC, prev_prev_prev_virtual_reg, mem, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
            else halt = r_func_ex(i+1, err_file, &PC, reg, mem, cur->r_type, cur->rs, cur->rt, cur->rd, cur->capital_c);
        }else if(cur->type == I) {
            if(need_forwarding) halt = i_func_ex(i+1, err_file, &PC, prev_prev_prev_virtual_reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);
            else halt = i_func_ex(i+1, err_file, &PC, reg, mem, cur->i_type, cur->rs, cur->rt, cur->capital_c);

        }
        halt1 += halt;
        if(halt1 != 0){
            //show_reg(snapshot, i+1);
            break;
        }

        if(stall == 0)id_ex = if_id;
        else id_ex = nop_inst;
//id:
        if(stall == 0 && flush == 0)if_id = cur_inst;
        else if(flush == 1)if_id = nop_inst;

        if(stall == 0) {
            PC+=4;
        }
        if(stall > 0)stall --;
        if(flush > 0)flush = 0;
        check();

        i++;
        need_forwarding = 0;
        if(forwarding[0] != 0) need_forwarding = 1;
        for(k = 0 ; k < 32; k++) prev_prev_prev_prev_virtual_reg[k] = prev_prev_prev_virtual_reg[k];
        for(k = 0 ; k < 32; k++) prev_prev_prev_virtual_reg[k] = prev_prev_virtual_reg[k];
        for(k = 0 ; k < 32; k++) prev_prev_virtual_reg[k] = prev_virtual_reg[k];
        if(stall == 0){

            for(k = 0 ; k < 32; k++) prev_virtual_reg[k] = virtual_reg[k];
        }

    }
}



void show_reg(FILE* output, unsigned int cycle){
    fprintf(output, "cycle %u\n", cycle);
    int i;
    char str[10];
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, reg[i]);
    }
    fprintf(output, "PC: 0x%08X\n", PC);

    fprintf(output, "IF: 0x%08X", cur_inst->val);
    if(stall > 0)fprintf(output, " to_be_stalled");
    if(flush > 0)fprintf(output, " to_be_flushed");
    fprintf(output, "\n");

    inst_to_str(str, if_id);
    fprintf(output, "ID: %s", str);
    if(stall > 0) fprintf(output, " to_be_stalled");
    else{
        if(bforwarding[0] == 1 && bforwarding[1] == 0 && ex_mem->type != NOP && ex_mem->is_bench == 0&& if_id->is_bench){
            //printf("EX-DM->type = %d\n", ex_mem->type);
//printf("PR BRENCH: ex_mem_write = %d, if_id_need_rs = %d, if_id_need_rt = %d ex = %d\n",ex_mem_write,if_id_need_rs,if_id_need_rt, ex_mem->type);
            if(bforwarding[2] == 2) fprintf(output, " fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d", bforwarding[3], bforwarding[3]);
            else fprintf(output, (bforwarding[2] == 0)? " fwd_EX-DM_rs_$%d": " fwd_EX-DM_rt_$%d", bforwarding[3]);
        }
        bforwarding[0] = 0;
        bforwarding[1] = 0;
        bforwarding[2] = 0;
        bforwarding[3] = 0;
    }
    fprintf(output, "\n");

    inst_to_str(str, id_ex);
    fprintf(output, "EX: %s", str);
    if(forwarding[0] == 1 && forwarding[1] == 0 && forwarding[2] != 1){
        if(forwarding[2] == 2) fprintf(output, " fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d", forwarding[3], forwarding[3]);
        else if(forwarding[2] == 0) fprintf(output, " fwd_EX-DM_rs_$%d", forwarding[3]);
        forwarding[0] = 0;
        forwarding[1] = 0;
        forwarding[2] = 0;
        forwarding[3] = 0;
    }
    if(mforwarding[0] == 1 && mforwarding[1] == 1 && mforwarding[2] != 1){
        if(mforwarding[2] == 2) fprintf(output, " fwd_DM-WB_rs_$%d fwd_DM-WB_rt_$%d", mforwarding[3], mforwarding[3]);
        else if(mforwarding[2] == 0) fprintf(output, " fwd_DM-WB_rs_$%d", mforwarding[3]);
        mforwarding[0] = 0;
        mforwarding[1] = 0;
        mforwarding[2] = 0;
        mforwarding[3] = 0;
    }
    if(forwarding[0] == 1 && forwarding[1] == 0 && forwarding[2] == 1){
        //if(forwarding[2] == 2) fprintf(output, " fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d", forwarding[3], forwarding[3]);
        if(forwarding[2] == 1) fprintf(output, " fwd_EX-DM_rt_$%d", forwarding[3]);
        forwarding[0] = 0;
        forwarding[1] = 0;
        forwarding[2] = 0;
        forwarding[3] = 0;
    }
    if(mforwarding[0] == 1 && mforwarding[1] == 1 && mforwarding[2] == 1){
        //if(mforwarding[2] == 2) fprintf(output, " fwd_DM-WB_rs_$%d fwd_DM-WB_rt_$%d", mforwarding[3], mforwarding[3]);
        if(mforwarding[2] == 1) fprintf(output, " fwd_DM-WB_rt_$%d", mforwarding[3]);
        mforwarding[0] = 0;
        mforwarding[1] = 0;
        mforwarding[2] = 0;
        mforwarding[3] = 0;
    }
    fprintf(output, "\n");

    inst_to_str(str, ex_mem);
    fprintf(output, "DM: %s\n", str);
    inst_to_str(str, mem_wb);
    fprintf(output, "WB: %s\n\n\n", str);

}


void show_virtual_reg(FILE* output, unsigned int cycle){
//if(cycle > 220) exit(-1);
    fprintf(output, "cycle %u\n", cycle);
    int i;
    fprintf(output, "\nvirtual_reg: PC = %08x\n", virtual_PC);
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, virtual_reg[i]);
    }
    fprintf(output, "\nprev_virtual_reg: \n");
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, prev_virtual_reg[i]);
    }
    fprintf(output, "\nprev_prev_virtual_reg: \n");
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, prev_prev_virtual_reg[i]);
    }
    fprintf(output, "\nprev_prev_prev_virtual_reg: \n");
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, prev_prev_prev_virtual_reg[i]);
    }
    fprintf(output, "\nprev_prev_prev_prev_virtual_reg: \n");
    for(i = 0 ; i < 32; i++){
        fprintf(output, "$%02u: 0x%08X\n", i, prev_prev_prev_prev_virtual_reg[i]);
    }
    fprintf(output, "\n\n\n");

}
void show_mem(FILE* output, unsigned int cycle){
    fprintf(output, "cycle %u\n", cycle);
    int i;
    for(i = 0 ; i < DIMAGE; i++){
        fprintf(output, "$%03X: 0x%08X\n", i, mem[i]);
    }
}
void check(){

        if(mem_wb->is_bench) mem_wb_write = -1;
        else if(mem_wb->type == R) mem_wb_write = mem_wb->rd;
        else if(mem_wb->type == I) mem_wb_write = mem_wb->rt;
        else if(mem_wb->type == JS && mem_wb->js_type == JAL) mem_wb_write = 31;
        else mem_wb_write = -1;

        if(ex_mem->is_bench) ex_mem_write = -1;
        else if(ex_mem->type == R && ex_mem->r_type != JR) ex_mem_write = ex_mem->rd;
        else if(ex_mem->type == I && ex_mem->i_type != SW && ex_mem->i_type != SH && ex_mem->i_type != SB && ex_mem->is_bench == 0) ex_mem_write = ex_mem->rt;
        else if(ex_mem->type == JS && ex_mem->js_type == JAL) ex_mem_write = 31;
        else ex_mem_write = -1;

        if(id_ex->is_bench) id_ex_write = -1;
        else if(id_ex->type == R && id_ex->r_type != JR) id_ex_write = id_ex->rd;
        else if(id_ex->type == I && id_ex->i_type != SW && id_ex->i_type != SH && id_ex->i_type != SB && id_ex->is_bench == 0) id_ex_write = id_ex->rt;
        else if(id_ex->type == JS && id_ex->js_type == JAL) id_ex_write = 31;
        else id_ex_write = -1;

        if(id_ex->type == R) {
            if(id_ex->r_type == SLL || id_ex->r_type == SRL || id_ex->r_type == SRA ){
                id_ex_need_rs = -2;
                id_ex_need_rt = id_ex->rt;
            }else if(id_ex->r_type == JR){
                id_ex_need_rt = -2;
                id_ex_need_rs = id_ex->rs;
            }else{
                id_ex_need_rs = id_ex->rs;
                id_ex_need_rt = id_ex->rt;
            }
        }else if(id_ex->type == I){
            if(id_ex->i_type == SW || id_ex->i_type == SH || id_ex->i_type == SB || id_ex->i_type == BEQ || id_ex->i_type == BNE){
                id_ex_need_rs = id_ex->rs;
                id_ex_need_rt = id_ex->rt;
            }else if(id_ex->i_type == LUI){
                id_ex_need_rs = -2;
                id_ex_need_rt = -2;
            }else{
                id_ex_need_rs = id_ex->rs;
                id_ex_need_rt = -2;
            }
        }else{
            id_ex_need_rs = -2;
            id_ex_need_rt = -2;
        }

        if(if_id->type == R) {
            if(if_id->r_type == SLL || if_id->r_type == SRL || if_id->r_type == SRA ){
                if_id_need_rs = -2;
                if_id_need_rt = if_id->rt;
            }else if(if_id->r_type == JR){
                if_id_need_rt = -2;
                if_id_need_rs = if_id->rs;
            }else{
                if_id_need_rs = if_id->rs;
                if_id_need_rt = if_id->rt;
            }
        }else if(if_id->type == I){
            if(if_id->i_type == SW || if_id->i_type == SH || if_id->i_type == SB || if_id->i_type == BEQ || if_id->i_type == BNE){
                if_id_need_rs = if_id->rs;
                if_id_need_rt = if_id->rt;
            }else if(if_id->i_type == LUI){
                if_id_need_rs = -2;
                if_id_need_rt = -2;
            }else{
                if_id_need_rs = if_id->rs;
                if_id_need_rt = -2;
            }
        }else{
            if_id_need_rs = -2;
            if_id_need_rt = -2;
        }
        if(id_ex_write != 0 && id_ex->load_use == 1 && (if_id_need_rs == id_ex_write || if_id_need_rt == id_ex_write)){
            stall=1;
        }else if(id_ex_write != 0 && if_id->is_bench == 1 && (if_id_need_rs == id_ex_write || if_id_need_rt == id_ex_write)){
            stall=1;
        }else if(ex_mem_write != 0 && if_id->is_bench == 1 && ex_mem->load_use == 1 && (if_id_need_rs == ex_mem_write || if_id_need_rt == ex_mem_write)){
            stall=1;
        }

        if(if_id->is_bench == 1 && stall == 0){
            if(if_id->is_beq == 1 && virtual_reg[if_id->rs] == virtual_reg[if_id->rt]&& stall == 0) flush = 1;
            else if(if_id->is_bne == 1 && virtual_reg[if_id->rs] != virtual_reg[if_id->rt]&& stall == 0) flush = 1;
            else if(if_id->is_jr == 1) flush = 1;
            else if(if_id->is_beq != 1 && if_id->is_bne != 1);
        }

        if((if_id->is_j == 1 || if_id->is_jr == 1 ) && stall == 0){
            flush = 1;
        }

        if(if_id->is_bench != 0 || if_id->is_jr){
            //printf("BRENCH: ex_mem_write = %d, if_id_need_rs = %d, if_id_need_rt = %d\n",ex_mem_write,if_id_need_rs,if_id_need_rt);
            if(ex_mem->load_use != 1 && (ex_mem->reg_write && ex_mem_write != 0)){
//printf("BRENCH: ex_mem_write = %d, if_id_need_rs = %d, if_id_need_rt = %d ex = %d\n",ex_mem_write,if_id_need_rs,if_id_need_rt, ex_mem->type);
                if(ex_mem_write == if_id_need_rs && ex_mem_write == if_id_need_rt){
                    bforwarding[0] = 1;
                    bforwarding[1] = 0;
                    bforwarding[2] = 2;
                    bforwarding[3] = ex_mem_write;
                }else if(ex_mem_write == if_id_need_rs){
                    bforwarding[0] = 1;
                    bforwarding[1] = 0;
                    bforwarding[2] = 0;
                    bforwarding[3] = ex_mem_write;
                }else if(ex_mem_write == if_id_need_rt){
//printf("BRENCH: ex_mem_write = %d, if_id_need_rs = %d, if_id_need_rt = %d ex = %d\n",ex_mem_write,if_id_need_rs,if_id_need_rt, ex_mem->type);
                    bforwarding[0] = 1;
                    bforwarding[1] = 0;
                    bforwarding[2] = 1;
                    bforwarding[3] = ex_mem_write;
                }
            }
        }
        if(id_ex->is_bench == 0){
             if(ex_mem->load_use != 1 && (ex_mem->reg_write && ex_mem_write != 0)){
                if(ex_mem_write == id_ex_need_rs && ex_mem_write == id_ex_need_rt){
                    forwarding[0] = 1;
                    forwarding[1] = 0;
                    forwarding[2] = 2;
                    forwarding[3] = ex_mem_write;
                }else if(ex_mem_write == id_ex_need_rs){
                    forwarding[0] = 1;
                    forwarding[1] = 0;
                    forwarding[2] = 0;
                    forwarding[3] = ex_mem_write;
                }else if(ex_mem_write == id_ex_need_rt){
                    forwarding[0] = 1;
                    forwarding[1] = 0;
                    forwarding[2] = 1;
                    forwarding[3] = ex_mem_write;
                }
            }
            if(mem_wb->reg_write && mem_wb_write!= 0){
                if(mem_wb_write == id_ex_need_rs && mem_wb_write == id_ex_need_rt && ex_mem_write != id_ex_need_rs && ex_mem_write != id_ex_need_rt){
                    mforwarding[0] = 1;
                    mforwarding[1] = 1;
                    mforwarding[2] = 2;
                    mforwarding[3] = mem_wb_write;
                }else if(mem_wb_write == id_ex_need_rs && ex_mem_write != id_ex_need_rs){
                    mforwarding[0] = 1;
                    mforwarding[1] = 1;
                    mforwarding[2] = 0;
                    mforwarding[3] = mem_wb_write;
                }else if(mem_wb_write == id_ex_need_rt && ex_mem_write != id_ex_need_rt){
                    mforwarding[0] = 1;
                    mforwarding[1] = 1;
                    mforwarding[2] = 1;
                    mforwarding[3] = mem_wb_write;
                }
            }
        }
}
