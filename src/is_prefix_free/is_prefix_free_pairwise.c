#include "huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*enumerate all codes to verify prefix_free*/
int is_prefix_free_pairwise(const char codes[][MAXL], int n){
    if(!satisfies_kraft(codes, n)){ // check kraft 
        return 0;
    }

    for(int i = 0 ; i < n-1 ; i++){
        for(int j = i + 1 ; j < n ; j++){
            // Either code can be the shorter prefix, regardless of input order.
            if(strncmp(codes[i], codes[j], strlen(codes[i])) == 0 ||
               strncmp(codes[j], codes[i], strlen(codes[j])) == 0){
                return 0; // failed :a code is prefix of another
            }
        }
    }
    return 1; // pass : no conflict
}
