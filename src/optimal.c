#include "huffman.h"
#include <stdlib.h>

#define HEAP_CAP 256

typedef struct{
    int data[HEAP_CAP];
    int size;
}MinHeap;

static MinHeap heap_init(void);
static void heap_push(MinHeap *h, int v);
static int heap_pop(MinHeap *h);

long long optimal_wpl(const int freq[], int n);
long long optimal_wpl_queue(const int freq[], int n);

static MinHeap heap_init(void){
    MinHeap h;
    h.size = 0;
    return h;
}

static void heap_push(MinHeap *h, int v){
    int i;
    // put v at the end first
    // then move it up while the parent is bigger
    for(i = h -> size; i > 0; i = (i - 1) / 2){
        if(v < h -> data[(i - 1) / 2]){
            h -> data[i] = h -> data[(i - 1) / 2];  // parent moves down
        }
        else{
            break;
        }
    }
    h -> data[i] = v;
    h -> size++;
}

static int heap_pop(MinHeap *h){
    int i = 0, m;
    int top = h -> data[0];                     // smallest to return
    int last = h -> data[h -> size - 1];        // last element will fill the last hole
    while(i * 2 + 1 < h -> size - 1){
        m = i * 2 + 1;                          // left child
        if(m + 1 < h -> size - 1 && h -> data[m] > h -> data[m + 1]){
            m = m + 1;                          // pick the smaller child
        }
        if(last > h -> data[m]){
            h -> data[i] = h -> data[m];        // smaller child moves up
            i = m;                              // hole moves down
        }
        else{
            break;
        }
    }
    h -> data[i] = last;                        // put last into the hole
    h -> size--;
    return top;
}

long long optimal_wpl(const int freq[], int n){
    long long total = 0;
    MinHeap HUF = heap_init();
    int i;
    long long a, b;
    for(i = 0; i < n; i++){
        heap_push(&HUF, freq[i]);               // all frequencies into the heap
    }
    while(HUF.size > 1){                        // always merge the two smallest
        a = heap_pop(&HUF);                     // a + b is the new node
        b = heap_pop(&HUF);
        total = total + a + b;
        heap_push(&HUF, a + b);
    }
    return total;
}

static int cmp_freq(const void *a, const void *b)
{
    const int x = *(const int *)a;
    const int y = *(const int *)b;
    if(x < y){
        return -1;
    }
    if(x > y){
        return 1;
    }
    return 0;
}

long long optimal_wpl_queue(const int freq[], int n){

    // Q1: sorted frequencies, still in increasing order
    // Q2: merged sums, always non-decreasing, so only the two heads matter
    int Q1[MAXN];
    int Q2[MAXN];
    int q2_head = 0, q2_tail = 0;
    
    long long total = 0;
    int i;

    for(i = 0; i < n; i++){
        Q1[i] = freq[i];
    }

    qsort(Q1, n, sizeof(int), cmp_freq);

    i = 0;
    while(n - i + q2_tail - q2_head > 1){       // more than one element left
        if(q2_head == q2_tail){
            total = total + Q1[i] + Q1[i + 1];
            Q2[q2_tail] = Q1[i] + Q1[i + 1];
            i = i + 2;
            q2_tail = q2_tail + 1;
        }
        else if(i >= n){
            total = total + Q2[q2_head] + Q2[q2_head + 1];
            Q2[q2_tail] = Q2[q2_head] + Q2[q2_head + 1];
            q2_head = q2_head + 2;
            q2_tail = q2_tail + 1;
        }
        else if(q2_head + 1 == q2_tail){
            if(i + 1 < n && Q1[i] <= Q2[q2_head] && Q1[i + 1] <= Q2[q2_head]){
                total = total + Q1[i] + Q1[i + 1];
                Q2[q2_tail] = Q1[i] + Q1[i + 1];
                i = i + 2;
                q2_tail = q2_tail + 1;
            }
            else{
                total = total + Q1[i] + Q2[q2_head];
                Q2[q2_tail] = Q1[i] + Q2[q2_head];
                i = i + 1;
                q2_head = q2_head + 1;
                q2_tail = q2_tail + 1;
            }
        }
        else{
            if(i + 1 < n && Q1[i] <= Q2[q2_head] && Q1[i + 1] <= Q2[q2_head]){
                total = total + Q1[i] + Q1[i + 1];
                Q2[q2_tail] = Q1[i] + Q1[i + 1];
                i = i + 2;
                q2_tail = q2_tail + 1;
            }
            else if(Q1[i] >= Q2[q2_head] && Q1[i] >= Q2[q2_head + 1]){
                total = total + Q2[q2_head] + Q2[q2_head + 1];
                Q2[q2_tail] = Q2[q2_head] + Q2[q2_head + 1];
                q2_head = q2_head + 2;
                q2_tail = q2_tail + 1;
            }
            else{
                total = total + Q1[i] + Q2[q2_head];
                Q2[q2_tail] = Q1[i] + Q2[q2_head];
                i = i + 1;
                q2_head = q2_head + 1;
                q2_tail = q2_tail + 1;
            }
        }
    }
    return total;
}