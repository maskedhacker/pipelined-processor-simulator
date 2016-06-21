#ifndef GET_IMAGES_H_INCLUDED
#define GET_IMAGES_H_INCLUDED

enum Type {R=1, I=2, JS=3, NOP=0};
enum R_type {ADD=0X20, SUB=0X22, AND=0X24, OR=0X25, XOR=0X26,
            NOR=0X27, NAND=0X28, SLT=0X2A, SLL=0X00, SRL=0X02,
            SRA=0X03, JR=0X08};
enum I_type {ADDI=0X08, LW=0X23, LH=0X21, LHU=0X25, LB=0X20,
            LBU=0X24, SW=0X2B, SH=0X29, SB=0X28, LUI=0X0F,
            ANDI=0X0C, ORI=0X0D, NORI=0X0E, SLTI=0X0A,
            BEQ=0X04, BNE=0X05};
enum JS_type {J=0X02, JAL=0X03, HALT=0X3F};

typedef struct INST {
    enum Type type;
    enum R_type r_type;
    enum I_type i_type;
    enum JS_type js_type;
    int val;
    int reg_write;
    int load_use;
    int is_bench;
    int is_beq;
    int is_bne;
    int is_jr;
    int is_j;
    int rs,rt,rd;
    int capital_c;

} INST;

int get_iimage(unsigned int* PC, INST** inst_arr); //return inst_num
int get_dimage(int* initialized_reg, int* mem_arr); //return d_mem_num
int add(int a, int b);

#endif // GET_IMAGES_H_INCLUDED
