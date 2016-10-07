#include <stdio.h>
#include <stdlib.h>

#include "jitter.h"

uint16_t* main_memory;


int main(int argc, char** argv){
    init_jit();
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
    
    jit_function  f = branch_to(0);
    f();

    return 0;
}
