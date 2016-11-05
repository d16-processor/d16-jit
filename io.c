//
//  io.c
//  d16-jit
//
//  Created by Michael Nolan on 11/5/16.
//  Copyright © 2016 Michael Nolan. All rights reserved.
//

#include "io.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
pthread_t io_thread;
int fd[2];
bool thread_run = true;
void* io_thread_process(void * unused){

	char buf[128];
	size_t bytes = 0;
	while(thread_run){
		while((bytes = read(fd[0], buf, sizeof(buf))) != EOF){
			fwrite(buf, 1, bytes, stdout);
		}
		fflush(stdout);
	}

return NULL;
}


void io_init(){
	pipe(fd);

	pthread_create(&io_thread, NULL, io_thread_process, NULL);
}
void cpu_thread_write_byte(uint8_t byte){
	write(fd[1], &byte, 1);

}
void io_destroy(){
	close(fd[1]);
	close(fd[0]);
	thread_run = false;
	pthread_join(io_thread, NULL);

}
