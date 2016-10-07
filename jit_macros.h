//
// Created by Michael Nolan on 10/6/16.
//

#ifndef D16_JIT_JIT_MACROS_H
#define D16_JIT_JIT_MACROS_H
#define INST_RR(x) {\
 jit_value_t res = x(func, rD, rS); \
jit_insn_store(func,rD,res); \
jit_insn_store_relative(func,state_ptr,rD_offset,rD); \
return 1; }

#define INST_RI(insn, const) {\
jit_value_t op2 = jit_value_create_nint_constant(func, jit_type_short, const);\
jit_value_t res = insn(func,rD,op2);\
jit_insn_store(func,rD,res);\
jit_insn_store_relative(func,state_ptr,rD_offset,rD);\
return 2; }

#define INST_MOVB(reg){ \
jit_value_t val = jit_value_create_nint_constant(func,jit_type_ushort,reg_sel);\
jit_insn_store(func,rD,val);\
jit_insn_store_relative(func,state_ptr,reg*2,rD);\
return 1;}


#endif //D16_JIT_JIT_MACROS_H
