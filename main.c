#include <stdio.h>
#include <stdlib.h>
#include <jit/jit.h>
#include "jitter.h"
jit_context_t context;
uint16_t* main_memory;


int main(int argc, char** argv){
    context = jit_context_create();
    if(argc != 2){
        fprintf(stderr,"Usage: d16-jit [binary]\n");
        exit(1);
    }
    FILE* bin = fopen(argv[1], "rb");
    if(bin == NULL){
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    main_memory = malloc(65535); //64K of memory
    fread(main_memory,1,65535,bin);
    fclose(bin);
    for(int i=0;i<10;i++){
        printf("0x%04x\n",*(main_memory+i));

    }
    branch_to(0);
    return 0;
}