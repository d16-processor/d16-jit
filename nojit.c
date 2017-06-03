#include "jitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_intrinsics.h"
#include "io.h"
extern uint16_t* main_memory;
extern bool trace_mode;
processor_state proc_state = {0};
uint16_t ip = 0;
int run_instruction(uint16_t*);

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
  uint16_t op1, op2;
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
  printf("Executing instruction %04x %d words\n", *instruction, retval);
  proc_state.instructions_executed += 1;
  switch(*instruction >> 8 & 0x7f){
  case ADD:
    proc_state.regs[rD] = op1+op2;
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
