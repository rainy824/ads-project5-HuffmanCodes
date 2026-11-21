#include "huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
** 原理：因为以同一个字符串为前缀的编码，排序后会聚在连续的一段里，而且这个前缀本身排在它们前面。只要它是后面某个编码的前缀，它就一定也是紧接着的那个编码的前缀。
*/

static int cmp(const void* a, const void* b){ // Comparison Function for quicksort
    const char *const *s1 = a;
    const char *const *s2 = b;
    return strcmp(*s1, *s2);
}


int is_prefix_free_sort(const char codes[][MAXL], int n){
    if(!satisfies_kraft(codes, n)){ // satisfy kraft? Yes: check carefully; No: directly failed
        return 0;
    }

    const char *order[MAXN]; // pointers to the original codes; sort pointers only
    for(int i = 0; i < n; i++){ // create a copyright
        order[i] = codes[i];
    }

    qsort(order, n, sizeof(order[0]), cmp); // sorted lexicographically
    for(int i = 0; i < n-1; i++){
        if(strncmp(order[i], order[i+1], strlen(order[i])) == 0){ 
            return 0;   // failed :order[i] is prefix of the next one
        }
    }
    return 1; // pass : no conflict
}
