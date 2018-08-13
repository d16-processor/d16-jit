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
pthread_t io_thread_write;
pthread_t io_thread_read;
int fd[2];
int read_fd[2];
bool thread_run = true;
volatile atomic_uint bytes_tx, bytes_rx;
volatile atomic_uint write_previous = 0;
void* io_thread_write_process(void * unused){

	char buf[128];
	size_t bytes = 0;
	while(thread_run){
		while((bytes = read(fd[0], buf, sizeof(buf))) != EOF){
			fwrite(buf, 1, bytes, stdout);
			fflush(stdout);
			unsigned int btx = 0;
			if(bytes > 0)
			  btx = atomic_fetch_add(&bytes_tx,bytes); 
			/*printf("bytes_tx (read) %d\n",btx);*/
		}
	}

return NULL;
}
void* io_thread_read_process(void* unused){
    char buf[128];
    size_t bytes = 0;
    /*close(read_fd[0]);*/
    /*setvbuf(stdin,NULL,_IONBF,0);*/
    while(thread_run){
        int spaces = atomic_load(&bytes_rx);
        if(spaces > 0){
            if((bytes = fread(buf,1,1,stdin)) != EOF){
                if(bytes != 0){
                    write(read_fd[1],buf,bytes);
                    atomic_fetch_sub(&bytes_rx,bytes);
                    /*fprintf(stderr,"Read byte: %c spaces: %d\n",*buf,spaces);*/
                }
            }
	    else {
	      char nul = 0;
	      write(read_fd[1],&nul, 1);
	      atomic_fetch_sub(&bytes_rx, bytes);
		
	    }
	}

    }
    return NULL;
}
uint8_t io_read_status(void){
    uint8_t status = 0;
    unsigned int btx = atomic_load(&bytes_tx);
    int brx = atomic_load(&bytes_rx);
    if(btx != 8 || brx == 8){
	int wr_prev = atomic_load(&write_previous);
	if(!wr_prev){
	    fd_set rfds;
	    FD_ZERO(&rfds);
	    FD_SET(read_fd[0],&rfds);
	    struct timeval timeout;
	    timeout.tv_sec = 0;
	    timeout.tv_usec = 10000;
	    select(1,&rfds, NULL, NULL, &timeout);
	}
	atomic_store(&write_previous,0);
        btx = atomic_load(&bytes_tx);
        brx = atomic_load(&bytes_rx);
    }
    status |= btx > 0 ? UART_TX_NOT_FULL : 0;
    status |= btx ==8 ? UART_TX_EMPTY : 0;
    status |= brx < 8 ? UART_RX_DATA_READY : 0;
    status |= brx < 0 ? UART_RX_OVERRUN : 0;
    
    return status;
}

void io_init(){
	pipe(fd);
    pipe(read_fd);
    atomic_store(&bytes_tx,8);
    atomic_store(&bytes_rx,8);
	pthread_create(&io_thread_write, NULL, io_thread_write_process, NULL);
    pthread_create(&io_thread_read, NULL, io_thread_read_process, NULL);
}
void cpu_thread_write_byte(uint8_t byte){
    
    unsigned int bfree = atomic_load(&bytes_tx);
    atomic_store(&write_previous, 1);
    if(bfree >0){
        while(!atomic_compare_exchange_strong(&bytes_tx,&bfree,bfree-1)){}
	    write(fd[1], &byte, 1);
    }

}
uint8_t cpu_thread_read_byte(void){
    int spaces = atomic_load(&bytes_rx);
    uint8_t res;
    if(spaces < 8){
        atomic_fetch_add(&bytes_rx,1);
        read(read_fd[0],&res,1);
        /*fprintf(stderr,"piped byte: %c spaces: %d\n",res,spaces);*/
    }
    return res;
}
void io_destroy(){
	close(fd[1]);
	close(fd[0]);

	thread_run = false;
	pthread_join(io_thread_write, NULL);
    pthread_cancel(io_thread_read);

}
