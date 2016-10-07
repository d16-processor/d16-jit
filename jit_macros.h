//
// Created by Michael Nolan on 10/6/16.
//

#ifndef D16_JIT_JIT_MACROS_H
#define D16_JIT_JIT_MACROS_H
|.macro inst_rr, x
| mov ax, [r12+(rS*2)]
| x word [r12+(rD*2)], ax
|.endmacro
|.macro inst_ri, x
| x word [r12+(rD*2)], imm
|.endmacro
|.macro cond_jmp, cond
|cond >1
|mov rdi, imm
|callp branch_to
|mov rsp,rbp
|pop rbp
|jmp rax
|1:
|.endmacro

#endif //D16_JIT_JIT_MACROS_H
