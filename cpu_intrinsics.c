//
//  cpu_intrinsics.c
//  d16-jit
//
//  Created by Michael Nolan on 10/8/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#include "cpu_intrinsics.h"



void io_store_word(uint16_t addr, uint16_t val){

}
void io_store_byte(uint16_t addr, uint8_t val){
	if(addr == 0xFF02){
		putc(val, stdout);
	}
}
uint16_t io_load_word(uint16_t addr){
	return 0;
}
uint8_t io_load_byte(uint16_t addr){
	return 0;
}
