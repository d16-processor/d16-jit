#include "jitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_intrinsics.h"
#include "io.h"
#define INST_RR(x) proc_state.regs[rD] = x; set_flags(x, op1, op2); break;
#define LOAD(addr) *(main_memory + ((addr)/2))
#define STORE(addr, data) *(main_memory+((addr)/2)) = data
#define FLAG_Z proc_state.flag_z
#define FLAG_C proc_state.flag_c
#define FLAG_N proc_state.flag_n
#define FLAG_V proc_state.flag_v

extern uint16_t* main_memory;
extern bool trace_mode;
processor_state proc_state = {0};
uint16_t ip = 0;
int run_instruction(uint16_t*);

void set_flags(int x, int o1, int o2){
  proc_state.flag_z = x == 0;
  proc_state.flag_c = (x & (1<<16)) != 0;
  proc_state.flag_n = x >= 0;
  proc_state.flag_v = ((o1 & (1<<15)) == (o2 & (1<<15))) && ((o1 & (1<<15)) != (x & (1<<15))); 
}
bool eval_cond(uint8_t condition){
  switch(condition){
  case EQ: return FLAG_Z;
  case NE: return !FLAG_Z;
  case OS: return FLAG_V;
  case OC: return !FLAG_V;
  case HI: return !FLAG_C & !FLAG_Z;
  case LS: return FLAG_C | FLAG_Z;
  case P : return !FLAG_N;
  case N : return FLAG_N;
  case CS: return FLAG_C;
  case CC: return !FLAG_C;
  case GE: return !FLAG_N ^ FLAG_V;
  case G : return (!FLAG_N ^ FLAG_V) && !FLAG_Z;
  case LE: return (FLAG_N ^ FLAG_V) | FLAG_Z;
  case L : return FLAG_N ^ FLAG_V;
  case AL: return 1;
  }
  return 0;
}

void finish(void){
  for(int i=0;i<8;i++){
    printf("r%d: %04x\n", i, proc_state.regs[i]);
  }
  printf("Instructions executed: %d\n", proc_state.instructions_executed);
  exit(0);
}

void run_from_ip(void* dummy){
  while(1){
    uint16_t* instruction = main_memory + (ip/2);
    int r = 0;
    if((r = run_instruction(instruction)) == -1){
      break;
    }
    ip += r*2;
  }
  finish();
}

int run_instruction(uint16_t* instruction){
  uint8_t rD, rS, condition;
  int op1, op2;
  uint8_t reg_sel = *instruction & 0xff;
  rD = reg_sel & 0x7;
  rS = reg_sel >> 3 & 0x7;
  condition = reg_sel >> 3 & 0xf;
  uint16_t immediate = *(instruction + 1);
  uint16_t inst = *instruction;
  int retval = 1;
  if(inst & (1<<15) && inst != 0xff00) { // immediate
    retval = 2;
    op2 = immediate;
    op1 = proc_state.regs[rD];
  } else { //no immediate
    retval = 1;
    op1 = proc_state.regs[rD];
    op2 = proc_state.regs[rS];
  }
  printf("Executing instruction %04x ZCPV %d%d%d%d\n", inst,
	 proc_state.flag_z,
	 proc_state.flag_c,
	 proc_state.flag_n,
	 proc_state.flag_v);
  proc_state.instructions_executed += 1;
  switch(*instruction >> 8 & 0x7f){
  case ADD:    INST_RR(op1+op2);
  case SUB:    INST_RR(op1-op2);
  case PUSH:
    proc_state.regs[7] -= 2;
    STORE(proc_state.regs[7], op2);
    break;
  case POP:
    proc_state.regs[rD] = LOAD(proc_state.regs[7]);
    proc_state.regs[7] += 2;
    break;
  case MOVB_R0:
  case MOVB_R1:
  case MOVB_R2:
  case MOVB_R3:
  case MOVB_R4:
  case MOVB_R5:
  case MOVB_R6:
  case MOVB_R7: //Intentional fallthrough
    rD = (inst >> 8) - MOVB_R0;
    op2 = inst & 0xff;
  case MOV:    INST_RR(op2);
  case AND:    INST_RR(op1&op2);
  case OR:     INST_RR(op1|op2);
  case XOR:    INST_RR(op1^op2);
  case NOT:    INST_RR(~op1);
  case NEG:    INST_RR(-op1);
    // LD, ST
  case CMP:
    set_flags(op1-op2, op1, op2); break;
  case JMP:
    if(eval_cond(condition)){
      if(inst&(1<<15)){
	ip = immediate;
      }else{
	ip = op1;
      }
      return 0;
    }
    break;

    
    
  case 0x7f:
    return -1;
    break;
  }
  return retval;
}



jit_function branch_to(uint16_t addr){
  ip = addr;
  return &run_from_ip;

}
