//
//  io.c
//  d16-jit
//
//  Created by Michael Nolan on 11/5/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//
#include "cpu_intrinsics.h"
#include "io.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
pthread_t io_thread;
int fd[2];
bool thread_run = true;
volatile atomic_uint bytes_tx;
void* io_thread_process(void * unused){

	char buf[128];
	size_t bytes = 0;
	while(thread_run){
		while((bytes = read(fd[0], buf, sizeof(buf))) != EOF){
			fwrite(buf, 1, bytes, stdout);
            unsigned int btx = 0;
           if(bytes > 1)
                btx = atomic_fetch_add(&bytes_tx,bytes); 
            /*printf("bytes_tx (read) %d\n",btx);*/
		}
		fflush(stdout);
	}

return NULL;
}
uint8_t io_read_status(void){
    uint8_t status = 0;
    unsigned int btx = atomic_load(&bytes_tx);
    status |= btx > 0 ? UART_TX_NOT_FULL : 0;
    status |= btx ==8 ? UART_TX_EMPTY : 0;
    return status;
}

void io_init(){
	pipe(fd);
    atomic_store(&bytes_tx,8);
	pthread_create(&io_thread, NULL, io_thread_process, NULL);
}
void cpu_thread_write_byte(uint8_t byte){
    unsigned int bfree = atomic_load(&bytes_tx);
    if(bfree >0){
        while(atomic_compare_exchange_strong(&bytes_tx,&bfree,bfree-1)){}
	    write(fd[1], &byte, 1);
    }

}
void io_destroy(){
	close(fd[1]);
	close(fd[0]);
	thread_run = false;
	pthread_join(io_thread, NULL);

}
