//
//  cpu_intrinsics.c
//  d16-jit
//
//  Created by Michael Nolan on 10/8/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#include "cpu_intrinsics.h"
#include "io.h"
#include "timer.h"
#include "sound.h"


void io_store_word(uint16_t addr, uint16_t val){
    switch(addr){
        case TIMER_DATA:
	  timer_set(val);
	  break;
        case SOUND_DATA:
	  IF_MIDI(sound_set(val));
	  break;
    }
}
void write_leds(uint8_t data){
    printf("LEDS: ");
    for(int i=128;i>0;i=i>>1){
        if(i&data)
            printf("1");
        else
            printf("0");
    }
    printf("\n");
}
void io_store_byte(uint16_t addr, uint8_t val){
	switch (addr){
        case IO_LED_DATA:
	  write_leds(val);
	  break;
	case IO_UART_DATA:
	  cpu_thread_write_byte(val);
	  break;
	default:
	  fprintf(stderr, "Write to invalid IO address: %x\n",addr);
	  break;
	}
}
uint16_t io_load_word(uint16_t addr){
    switch(addr){
        case TIMER_DATA:
            return timer_get();
        default:
            fprintf(stderr,"Read from invalid IO address: %x\n",addr);
            return 0;
    }
}
uint8_t io_load_byte(uint16_t addr){
    switch(addr){
        case IO_UART_STATUS:
            return io_read_status();
        case IO_UART_DATA:
            return cpu_thread_read_byte();
        default:
            fprintf(stderr, "Read from invalid IO address: %x\n",addr);
            break;
	}
	return 0;
}
