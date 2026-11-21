#include "huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
** 1. trie is implemented by tree
** 2. initialize new node with NULL for children and 0 for is_end
*/

typedef struct TrieNode{
    int is_end;
    struct TrieNode* left;  // branch 0
    struct TrieNode* right; // branch 1
} TrieNode;
typedef TrieNode* ptrToTrieNode;

static ptrToTrieNode CreateNode(void){
    ptrToTrieNode newNode = (ptrToTrieNode)calloc(1, sizeof(TrieNode));
    if(newNode == NULL){
        return NULL;
    }
    newNode->is_end = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

static void FreeTrie(ptrToTrieNode node){
    if(node == NULL){
        return;
    }
    FreeTrie(node->left);
    FreeTrie(node->right);
    free(node);
}

int is_prefix_free_tree(const char codes[][MAXL], int n){
    if(!satisfies_kraft(codes, n)){
        return 0;
    }

    ptrToTrieNode root = CreateNode();
    if(root == NULL){
        return 0;
    }

    for(int i = 0; i < n; i++){
        int k = 0;
        ptrToTrieNode p = root;
        while(codes[i][k] != '\0'){
            if(p->is_end == 1){
                FreeTrie(root);
                return 0;
            }

            if(codes[i][k] == '0'){ // go to branch 0(left)
                if(!p->left){
                    p->left = CreateNode();
                    if(p->left == NULL){
                        FreeTrie(root);
                        return 0;
                    }
                }
                p = p->left;
            }else{                  // go to branch 1(right)
                if(!p->right){
                    p->right = CreateNode();
                    if(p->right == NULL){
                        FreeTrie(root);
                        return 0;
                    }
                }
                p = p->right;
            }
            k += 1; // move ptr on the code
        }

        // travel to the end of a code
        if(p->is_end == 1){ // failed :codes dumplicated
            FreeTrie(root);
            return 0;
        }else if(p->left || p->right){ // failed :failed :new codes is prefix of old one
            FreeTrie(root);
            return 0;
        }else{
            p->is_end = 1; // denote this node is an end
        }
    }
    FreeTrie(root);
    return 1; // pass :no conflict
}
