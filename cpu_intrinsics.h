//
//  cpu_intrinsics.h
//  d16-jit
//
//  Created by Michael Nolan on 10/8/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#ifndef cpu_intrinsics_h
#define cpu_intrinsics_h
#include <stdint.h>
#include <stdio.h>
void io_store_word(uint16_t addr, uint16_t val);
void io_store_byte(uint16_t addr, uint8_t val);
uint16_t io_load_word(uint16_t addr);
uint8_t io_load_byte(uint16_t addr);
#endif /* cpu_intrinsics_h */