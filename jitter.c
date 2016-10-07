//
// Created by Michael Nolan on 10/6/16.
//

#include "jitter.h"
#include <jit/jit.h>
#include <stdio.h>
#include <stdlib.h>
#include "jit_macros.h"

extern jit_context_t context;
processor_state state = {0};
void* state_ptr = &state;
void finish();
extern uint16_t* main_memory;
int jit_instruction(uint16_t* insn, jit_value_t state, jit_function_t func);
void branch_to(uint16_t addr){
    uint16_t* instructions = main_memory + (addr/2);
    jit_context_build_start(context);
    jit_type_t params[1];
    params[0] = jit_type_void_ptr; //This is the pointer to the global processor state
    jit_type_t signature = jit_type_create_signature(
        jit_abi_cdecl, //abi
        jit_type_void, //ret type
        params,
        1, //inputs
        0 //return
    );
    jit_function_t function = jit_function_create(context, signature);
    jit_value_t state_ptr;
    state_ptr = jit_value_get_param(function, 0);
    while(*instructions < 0xff00){
        instructions += jit_instruction(instructions, state_ptr, function);
    }
    jit_type_t exit_params[1] = {jit_type_int};
    jit_type_t exit_signature = jit_type_create_signature(jit_abi_cdecl,jit_type_void,exit_params,0,0);
    jit_insn_call_native(function,"finish",&finish,exit_signature,NULL,0,0);
    jit_context_build_end(context);

    jit_function_compile(function);
    typedef void (*func_t)(void*);
    func_t func = jit_function_to_closure(function);
    func(&state);


}

int jit_instruction(uint16_t* insn, jit_value_t state_ptr, jit_function_t func){
    uint8_t opcode = *insn >> 8;
    uint8_t reg_sel = *insn & 0xFF;
    jit_nint rD_offset = (reg_sel&0x7) * 2;
    jit_nint rS_offset = ((reg_sel >>3)&0x7) *2;
    jit_value_t rD = jit_insn_load_relative(func, state_ptr, rD_offset, jit_type_short);
    jit_value_t rS = jit_insn_load_relative(func, state_ptr, rS_offset, jit_type_short);
    switch(opcode){
        case ADD: //ADD
            INST_RR(jit_insn_add)


        case SUB:  //SUB
            INST_RR(jit_insn_sub)
        case MOVB_R0: INST_MOVB(0)
        case MOVB_R1: INST_MOVB(1)
        case MOVB_R2: INST_MOVB(2)
        case MOVB_R3: INST_MOVB(3)
        case MOVB_R4: INST_MOVB(4)
        case MOVB_R5: INST_MOVB(5)
        case MOVB_R6: INST_MOVB(6)
        case MOVB_R7: INST_MOVB(7)
        case MOV:
            jit_insn_store_relative(func,state_ptr,rD_offset,rS);
            return 1;
        case ADDI:  //ADDI
            INST_RI(jit_insn_add,(*(insn+1)))

        case SUBI:  //SUBI
            INST_RI(jit_insn_sub,(*(insn+1)))


    }
    return 1;
}
void finish(){
    for(int i=0;i<8;i++){
        printf("r%d: 0x%04x\n",i,state.regs[i]);
    }
    exit(0);
}