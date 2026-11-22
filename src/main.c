#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "huffman.h"

static long long now_ns(void)
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return (long long)now.tv_sec * 1000000000 + now.tv_nsec;
}

int main(int argc, char *argv[])
{
    const char *optimal_name = argc > 1 ? argv[1] : "heap";
    const char *prefix_name = argc > 2 ? argv[2] : "array";
    char *repeat_end = NULL;
    long repeat = argc > 3 ? strtol(argv[3], &repeat_end, 10) : 1;
    int timed = argc > 3;

    long long (*optimal)(const int[], int) = optimal_wpl;
    if (strcmp(optimal_name, "queue") == 0)
        optimal = optimal_wpl_queue;
    else if (strcmp(optimal_name, "heap") != 0)
        optimal = NULL;

    const struct {
        const char *name;
        int (*check)(const char[][MAXL], int);
    } methods[] = {
        {"array", is_prefix_free},
        {"tree", is_prefix_free_tree},
        {"sort", is_prefix_free_sort},
        {"pairwise", is_prefix_free_pairwise},
    };
    int (*prefix_free)(const char[][MAXL], int) = NULL;
    for (size_t i = 0; i < sizeof methods / sizeof methods[0]; i++) {
        if (strcmp(prefix_name, methods[i].name) == 0)
            prefix_free = methods[i].check;
    }

    if (argc > 4 || !optimal || !prefix_free ||
        (timed && (*repeat_end || repeat < 1 || repeat >= INT_MAX))) {
        fprintf(stderr, "Usage: %s [heap|queue [array|tree|sort|pairwise [repeat]]]\n",
                argv[0]);
        return 2;
    }

    int n, m;
    int freq[MAXN];
    int index_by_char[UCHAR_MAX + 1] = {0};
    char codes[MAXN][MAXL];
    char symbol;
    if (scanf("%d", &n) != 1)
        return 1;
    for (int i = 0; i < n; i++) {
        if (scanf(" %c %d", &symbol, &freq[i]) != 2)
            return 1;
        index_by_char[(unsigned char)symbol] = i + 1;
    }
    if (scanf("%d", &m) != 1)
        return 1;

    long long start = timed ? now_ns() : 0;
    long long best = 0;
    for (long i = 0; i < repeat; i++)
        best = optimal(freq, n);
    long long optimal_ns = timed ? now_ns() - start : 0;
    long long prefix_ns = 0;
    long long prefix_calls = 0;

    for (int submission = 0; submission < m; submission++) {
        int seen[MAXN] = {0};
        int valid = 1;
        long long cost = 0;
        for (int i = 0; i < n; i++) {
            if (scanf(" %c %63s", &symbol, codes[i]) != 2)
                return 1;
            int index = index_by_char[(unsigned char)symbol];
            if (index == 0 || seen[index]) {
                valid = 0;
                continue;
            }
            seen[index] = 1;
            cost += (long long)freq[index - 1] * (int)strlen(codes[i]);
        }

        int accepted = 0;
        if (valid && cost == best) {
            start = timed ? now_ns() : 0;
            for (long i = 0; i < repeat; i++)
                accepted = prefix_free((const char (*)[MAXL])codes, n);
            if (timed) {
                prefix_ns += now_ns() - start;
                prefix_calls += repeat;
            }
        }
        puts(accepted ? "Yes" : "No");
    }

    if (timed)
        fprintf(stderr, "%ld %lld %lld %lld\n", repeat, optimal_ns,
                prefix_calls, prefix_ns);
    return 0;
}
