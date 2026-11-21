#include "huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
** 1. trie is implemented by array
** 2. index of root is 0
** 3. child[b] == -1 means no branch
** 4. initialize new node with -1 for child[] and 0 for is_end
*/

typedef struct{ 
    int is_end; // 1：one complete codes end here, otherwise 0
    int child[2]; // the index of its children within nodes array
} TrieNode;

int is_prefix_free(const char codes[][MAXL], int n){
    if(!satisfies_kraft(codes, n)){
        return 0;
    }

    TrieNode trie[MAXN * MAXL];
    trie[0].child[0] = -1;
    trie[0].child[1] = -1;
    trie[0].is_end = 0;

    int node_cnt = 1; // count total node in trie and indicate next available position
    for(int i = 0; i < n; i++){
        int k = 0, p = 0;
        while(codes[i][k] != '\0'){
            if(trie[p].is_end == 1){ // failed :old codes is prefix of new one 
                return 0;
            }

            int b = codes[i][k] - '0'; // denote the kth bit
            if(trie[p].child[b] == -1){ // no branch
                // create a new branch
                int q = node_cnt++;
                trie[q].is_end = 0;
                trie[q].child[0] = -1;
                trie[q].child[1] = -1;
                // connect new branch with parent
                trie[p].child[b] = q;
            } 
            p = trie[p].child[b];
            k++;
        }

        // travel to the end of a code
        if(trie[p].is_end == 1){ // failed :codes dumplicated
            return 0;
        }else if(trie[p].child[0] != -1 || trie[p].child[1] != -1){ // failed :new codes is prefix of old one
            return 0;
        }else{
            trie[p].is_end = 1; // denote this node is an end
        }
    }
    return 1; // pass : no conflict
}
