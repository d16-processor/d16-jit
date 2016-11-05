//
//  io.h
//  d16-jit
//
//  Created by Michael Nolan on 11/5/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#ifndef io_h
#define io_h

#include <stdio.h>
#include <stdint.h>
void io_init();
void cpu_thread_write_byte(uint8_t byte);
void io_destroy();
#endif /* io_h */
