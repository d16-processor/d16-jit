#include "jitter.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu_intrinsics.h"
#include "io.h"
#include "sound.h"
#define INST_RR(x) proc_state.regs[rD] = x; set_flags(x);
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


void set_flags(int x){
  proc_state.flag_z = x == 0;
  proc_state.flag_c = (x & (1<<16)) != 0;
  proc_state.flag_n = x >= 0;
  proc_state.flag_v = 0;
}
void set_v_flag(int x, int o1, int o2){
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
  io_destroy();
  IF_MIDI(cleanup_sound());
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
  /* printf("Executing instruction %04x ZCPV %d%d%d%d\n", inst, */
  /* 	 proc_state.flag_z, */
  /* 	 proc_state.flag_c, */
  /* 	 proc_state.flag_n, */
  /* 	 proc_state.flag_v); */
  proc_state.instructions_executed += 1;
  switch(*instruction >> 8 & 0x7f){
  case ADD:    INST_RR(op1+op2); set_v_flag(op1+op2,op1,op2); break;
  case SUB:    INST_RR(op1-op2); set_v_flag(op1-op2,op1,-op2); break;
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
  case MOV:    INST_RR(op2); break;
  case AND:    INST_RR(op1&op2); break;
  case OR:     INST_RR(op1|op2); break;
  case XOR:    INST_RR(op1^op2); break;
  case NOT:    INST_RR(~op1); break;
  case NEG:    INST_RR(-op1); break;
  case LD:
  case ST:
    {
    uint16_t addr;
    if(inst&(1<<6)){		/* Displacement */
      addr = proc_state.regs[rS] + immediate;
    } else {			/* Regular */
      addr = op2;
    }
    if(addr < 0xff00){
      if(((inst>>8) & 0x7f) == ST){
	if(inst&(1<<7)){		/* Byte */
	  *((uint8_t*) main_memory + addr) = (uint8_t) proc_state.regs[rD] & 0xff;
	} else {			/* Word */
	  *(main_memory + addr/2) = proc_state.regs[rD];
	}
      } else { //LD
	if(inst&(1<<7)){
	  proc_state.regs[rD] = *((uint8_t*)main_memory + addr);
	} else {
	  proc_state.regs[rD] = *(main_memory+addr/2);
	}
      }
    } else {
      if(((inst>>8) & 0x7f) == ST){
	if(inst&(1<<7)) io_store_byte(addr, proc_state.regs[rD]);
	else            io_store_word(addr, proc_state.regs[rD]);
      }else{
	if(inst&(1<<7)) proc_state.regs[rD] = io_load_byte(addr);
	else            proc_state.regs[rD] = io_load_word(addr);
      }
    }
    break;
  }
  case CMP:
    set_flags(op1-op2); set_v_flag(op1-op2, op1, -op2); break;
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
  case CALL:
    if(eval_cond(condition)){
      proc_state.link_register = ip + retval*2;
      if(inst&(1<<15)){
	ip=immediate;
      } else {
	ip=op1;
      }
      return 0;
    }
    break;
  case SPEC: //ret
    ip = proc_state.link_register;
    return 0;
    break;
  case SHL:    INST_RR((uint16_t)op1 << (op2 & 0xf)); break;
  case SHR:    INST_RR((uint16_t)op1 >> (op2 & 0xf)); break;
  case ROL:    INST_RR((uint16_t)op1 << (op2 & 0xf) | (uint16_t)op1 >> (16 - (op2 & 0xf)));
  case RCL:
    op1 |= FLAG_C << 16;
    INST_RR((uint32_t)op1 << (op2 & 0xf) | (uint32_t)op1 >> (17 - (op2 & 0xf)));
    break;
  case ADC:    INST_RR(op1+op2+FLAG_C); set_v_flag(op1+op2+FLAG_C, op1, op2+FLAG_C); break;
  case SBB:    INST_RR(op1-op2-FLAG_C); set_v_flag(op1-op2-FLAG_C, op1, op2-FLAG_C); break;
  case SET:
    if(eval_cond(condition)){
      INST_RR(1);
    } else {
      INST_RR(0);
    }
    break;
  case PUSHLR:
    proc_state.regs[7] -= 2;
    STORE(proc_state.regs[7], proc_state.link_register);
    break;
  case SAR: INST_RR((int16_t)op1 >> op2); break;
    
    
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
