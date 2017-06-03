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
#define IO_LED_DATA 0xff00
#define IO_UART_DATA 0xff02
#define IO_UART_STATUS 0xff03
#define IO_UART_BAUD 0xff04
#define TIMER_DATA 0xff06
#define SOUND_DATA 0xff08

#define UART_TX_NOT_FULL 1
#define UART_TX_EMPTY 2
#define UART_RX_DATA_READY 4
#define UART_RX_OVERRUN 8

void io_store_word(uint16_t addr, uint16_t val);
void io_store_byte(uint16_t addr, uint8_t val);
uint16_t io_load_word(uint16_t addr);
uint8_t io_load_byte(uint16_t addr);
#endif /* cpu_intrinsics_h */
