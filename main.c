#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <getopt.h>
#include "io.h"
#include "jitter.h"
#include "sound.h"


bool trace_mode = false;
uint16_t* main_memory;
#define FMT_STRING "t"
void finish(int);
int main(int argc, char** argv){
    setenv("POSIXLY_CORRECT", "1",1);
    int c;
    FILE* bin = NULL;
    while(optind < argc){
        if((c = getopt(argc,argv, FMT_STRING)) != -1){
            switch(c){
                case 't':
                    trace_mode = true;
                    break;
            }
        }
        else{
            if(bin == NULL){
                bin = fopen(argv[optind],"rb");
            }
            optind++;
        }
    }
    io_init();
    IF_MIDI(init_sound());
    signal(SIGINT, finish);
    JIT_ONLY(init_jit();)
    
    
    if(bin == NULL){
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    main_memory = malloc(65535); //64K of memory
    fread(main_memory,1,65535,bin);
    fclose(bin);
    
    jit_function  f = branch_to(0);
    f(f);

    return 0;
}
