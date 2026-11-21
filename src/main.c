#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

typedef struct{
    char c;
    int freq;
}CharInfo;

int main(){

    int N, M, i, j;

    CharInfo info[MAXN];
    int freq[MAXN];

    char codes[MAXN][MAXL];
    char buf[MAXL];
    char c;
    long long cost;

    scanf("%d", &N);

    for(i = 0; i < N; i++){
        scanf(" %c %d", &info[i].c, &info[i].freq);
        freq[i] = info[i].freq;
    }

    long long best = optimal_wpl(freq, N);

    scanf("%d", &M);

    for (j = 0; j < M; j++) {

        cost = 0;

        for(i = 0; i < N; i++){
            scanf(" %c %63s", &c, buf);
            strcpy(codes[i], buf);
            int len = (int)strlen(buf);

            // info[i] pairs with line i: submissions are assumed to list the
            // characters in the same order as the input c[i].
            cost = cost + (long long)info[i].freq * len;
        }

        // the total length must reach the theoretical minimum
        if(cost != best){
            printf("No\n");
            continue;
        }

        // no code may be a prefix of another
        if(is_prefix_free(codes, N) == 0){
            printf("No\n");
            continue;
        }

        printf("Yes\n");
    }
    
    return 0;
}