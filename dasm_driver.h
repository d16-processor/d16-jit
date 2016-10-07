//
// Created by Michael Nolan on 10/7/16.
//

#ifndef D16_JIT_DASM_DRIVER_H
#define D16_JIT_DASM_DRIVER_H

#include "dynasm/dasm_proto.h"


void initjit(dasm_State **state, const void *actionlist);
void *jitcode(dasm_State **state);
void free_jitcode(void *code);
#endif //D16_JIT_DASM_DRIVER_H
