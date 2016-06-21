#include <stdio.h>
#include "get_images.h"
#include <string.h>
void inst_to_str(char str[], INST* inst){

    if(inst->type == NOP) strcpy(str, "NOP");
    else if(inst->type == R){
        switch(inst->r_type){
            case ADD:
                strcpy(str, "ADD");
                break;
            case SUB:
                strcpy(str, "SUB");
                break;
            case AND:
                strcpy(str, "AND");
                break;
            case OR:
                strcpy(str, "OR");
                break;
            case XOR:
                strcpy(str, "XOR");
                break;
            case NOR:
                strcpy(str, "NOR");
                break;
            case NAND:
                strcpy(str, "NAND");
                break;
            case SLT:
                strcpy(str, "SLT");
                break;
            case SLL:
                strcpy(str, "SLL");
                break;
            case SRL:
                strcpy(str, "SRL");
                break;
            case SRA:
                strcpy(str, "SRA");
                break;
            case JR:
                strcpy(str, "JR");
                break;
            default:
                break;
        }
    }else if(inst->type == I){
        switch(inst->i_type){
            case ADDI:
                strcpy(str, "ADDI");
                break;
            case LW:
                strcpy(str, "LW");
                break;
            case LH:
                strcpy(str, "LH");
                break;
            case LHU:
                strcpy(str, "LHU");
                break;
            case LB:
                strcpy(str, "LB");
                break;
            case LBU:
                strcpy(str, "LBU");
                break;
            case SW:
                strcpy(str, "SW");
                break;
            case SH:
                strcpy(str, "SH");
                break;
            case SB:
                strcpy(str, "SB");
                break;
            case LUI:
                strcpy(str, "LUI");
                break;
            case ANDI:
                strcpy(str, "ANDI");
                break;
            case ORI:
                strcpy(str, "ORI");
                break;
            case NORI:
                strcpy(str, "NORI");
                break;
            case SLTI:
                strcpy(str, "SLTI");
                break;
            case BEQ:
                strcpy(str, "BEQ");
                break;
            case BNE:
                strcpy(str, "BNE");
                break;
            default:
                break;
        }

    }else if(inst->type == JS){
        switch(inst->js_type){
            case J:
                strcpy(str, "J");
                break;
            case JAL:
                strcpy(str, "JAL");
                break;
            case HALT:
                strcpy(str, "HALT");
                break;
            default:
                break;
        }
    }
}
