#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAXN 64    // max number of characters (N <= 63)
#define MAXL 64    // 63 bits + '\0'

// min total code length in bits 
// freq is read-only
// by member A
long long optimal_wpl(const int freq[], int n);

// same result as the heap version, sorted array + two queues
// optional: used for cross-checking and comparison
// by member A
long long optimal_wpl_queue(const int freq[], int n);

// 1 = no code is a prefix of another, 0 = prefix conflict
// prefix validity only; missing/duplicate/unknown chars are handled by main
// by member B
int is_prefix_free(const char codes[][MAXL], int n);

// Alternative implementations, with the same result as the array Trie above.
int is_prefix_free_sort(const char codes[][MAXL], int n);
int is_prefix_free_pairwise(const char codes[][MAXL], int n);
// The pointer Trie also returns 0 if memory allocation fails.
int is_prefix_free_tree(const char codes[][MAXL], int n);

// Necessary condition only: 1 means sum(2^(-length)) <= 1.
// Passing this check does NOT prove that the given codes are prefix-free.
// Inputs follow the problem: 2 <= n <= 63, nonempty binary strings <= 63 bits.
int satisfies_kraft(const char codes[][MAXL], int n);

#endif
