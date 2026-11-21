#include "huffman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
** 1. trie is implemented by pointers
** 2. initialize new node with NULL for children and 0 for is_end
** 3. leftChild denotes '0', rightChile denotes '1' 
*/

typedef struct TrieNode{
    int is_end;             // flag: is an end for a previous code or not
    struct TrieNode* left;  // branch 0
    struct TrieNode* right; // branch 1
} TrieNode;
typedef TrieNode* ptrToTrieNode;

static ptrToTrieNode CreateNode(void){  // Create a new Node on trie
    ptrToTrieNode newNode = (ptrToTrieNode)calloc(1, sizeof(TrieNode));
    if(newNode == NULL){ // Out of Space
        return NULL;
    }
    newNode->is_end = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;     // a ptr to new Node
}

static void FreeTrie(ptrToTrieNode node){   // free the trie recursively
    if(node == NULL){
        return;
    }
    FreeTrie(node->left);
    FreeTrie(node->right);
    free(node);
}

int is_prefix_free_tree(const char codes[][MAXL], int n){
    if(!satisfies_kraft(codes, n)){ // satisfy kraft? Yes: check carefully; No: directly failed
        return 0;
    }

    ptrToTrieNode root = CreateNode();  // Create a root
    if(root == NULL){
        return 0;
    }

    for(int i = 0; i < n; i++){ // traverse all codes
        int k = 0;  // a ptr on specific code
        ptrToTrieNode p = root;
        while(codes[i][k] != '\0'){ // traverse a specific code and explore on the trie

            if(p->is_end == 1){ // failed :there exist an old code being the prefix of new one
                FreeTrie(root);
                return 0;
            }

            if(codes[i][k] == '0'){ // go to branch 0(left)
                if(!p->left){   // no branch 0: create  
                    p->left = CreateNode();
                    if(p->left == NULL){
                        FreeTrie(root);
                        return 0;
                    }
                }
                p = p->left;    // explore to branch 0
            }else{                  // go to branch 1(right)
                if(!p->right){  // no branch 1: create
                    p->right = CreateNode();
                    if(p->right == NULL){
                        FreeTrie(root);
                        return 0;
                    }
                }
                p = p->right;   // explore to branch 1
            }
            k += 1; // move ptr on the code
        }

        // travel to the end of a specific code
        if(p->is_end == 1){ // failed :codes duplicated
            FreeTrie(root);
            return 0;
        }else if(p->left || p->right){ // failed :new codes is prefix of old one
            FreeTrie(root);
            return 0;
        }else{
            p->is_end = 1; // denote this node is an end
        }
    }
    FreeTrie(root);
    return 1; // pass :no conflict
}
