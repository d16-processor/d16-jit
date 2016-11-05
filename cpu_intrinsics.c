//
//  cpu_intrinsics.c
//  d16-jit
//
//  Created by Michael Nolan on 10/8/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#include "cpu_intrinsics.h"
#include "io.h"

#define UART_TX_NOT_FULL 1
#define UART_TX_EMPTY 2
#define UART_RX_DATA_READY 4
#define UART_RX_OVERRUN 8

void io_store_word(uint16_t addr, uint16_t val){

}
void io_store_byte(uint16_t addr, uint8_t val){
	switch (addr){
		case IO_UART_DATA:
			cpu_thread_write_byte(val);
			break;
		default:
			fprintf(stderr, "Write to invalid IO address: %x\n",addr);
			break;
	}
}
uint16_t io_load_word(uint16_t addr){
	return 0;
}
uint8_t io_load_byte(uint16_t addr){
	if(addr == IO_UART_STATUS){
		uint8_t status = 0;
		status |= UART_TX_NOT_FULL;
		return status;
	}
	return 0;
}
