#include "jitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_intrinsics.h"
#include "io.h"
#define INST_RR(x) proc_state.regs[rD] = x; set_flags(x, op1, op2); break;
#define LOAD(addr) *(main_memory + ((addr)/2))
#define STORE(addr, data) *(main_memory+((addr)/2)) = data
extern uint16_t* main_memory;
extern bool trace_mode;
processor_state proc_state = {0};
uint16_t ip = 0;
int run_instruction(uint16_t*);

void set_flags(int x, int o1, int o2){
  proc_state.flag_z = x == 0;
  proc_state.flag_c = (x & (1<<16)) != 0;
  proc_state.flag_p = x >= 0;
  proc_state.flag_v = ((o1 & (1<<15)) == (o2 & (1<<15))) && ((o1 & (1<<15)) != (x & (1<<15))); 
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
  int retval = 1;
  if(*instruction & (1<<15) && *instruction != 0xff00) { // immediate
    retval = 2;
    op2 = immediate;
    op1 = proc_state.regs[rD];
  } else { //no immediate
    retval = 1;
    op1 = proc_state.regs[rD];
    op2 = proc_state.regs[rS];
  }
  printf("Executing instruction %04x ZCPV %d%d%d%d\n", *instruction,
	 proc_state.flag_z,
	 proc_state.flag_c,
	 proc_state.flag_p,
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
    rD = (*instruction >> 8) - MOVB_R0;
    op2 = *instruction & 0xff;
  case MOV:    INST_RR(op2);
  case AND:    INST_RR(op1&op2);
  case OR:     INST_RR(op1|op2);
  case XOR:    INST_RR(op1^op2);
  case NOT:    INST_RR(~op1);
  case NEG:    INST_RR(-op1);
    
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
