#include "huffman.h"
#include <stdint.h>
#include <string.h>

int satisfies_kraft(const char codes[][MAXL], int n)
{
    /* Scale sum(2^(-length)) <= 1 by 2^63, using exact unsigned integers. */
    uint64_t remaining = UINT64_C(1) << (MAXL - 1);

    for (int i = 0; i < n; i++) {
        size_t length = strlen(codes[i]);
        if (length == 0 || length >= MAXL) {
            return 0;
        }

        uint64_t required = UINT64_C(1) << (MAXL - 1 - length);
        if (required > remaining) {
            return 0;
        }
        /* Subtract only after checking: no overflowing sum or underflow. */
        remaining -= required;
    }
    return 1;
}
