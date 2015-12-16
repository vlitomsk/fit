#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

/* enum {false, true} занимает 4 байта, поэтому так */
#define true (1)
#define false (0)
typedef uint8_t bool;

#define MIN_DWORDS 256
#define MAX_DWORDS (8*1024*1024)
#define PASS_COUNT 10

void nextArrSize(size_t *arrSize) {
    *arrSize = floor((float)(*arrSize) / 1.2f);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

double traverse(const uint32_t *arena, size_t arrSize) {
    union ticks{ 
        uint64_t t64;
        struct s32 { uint32_t th, tl; } t32;
    } start, end;
    volatile size_t k, i, pass;
    double minCycles = 10e8;
    for (k = 0, i = 0; i < arrSize; ++i) // heating array 
        k = arena[k];
    for (pass = 0; pass < PASS_COUNT; ++pass) {
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (k = 0, i = 0; i < arrSize; ++i)
            k = arena[k];
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        minCycles = min(minCycles, (end.t64 - start.t64) / (double)arrSize);
    }
    return minCycles;
}

void printSize(FILE* log, size_t dwords) {
    fprintf(log, "%lf", (double)dwords / 256.0);
}

/* RAND_MAX is guaranteed to be not less than 32768 = 2^15 */
/* RAND_MAX^2 not less than 2^30 */
size_t bigRand(void) {
    return rand() * rand();
}

int main() {
    size_t i, arrSize;
    uint32_t *arena = (uint32_t*)malloc(MAX_DWORDS * sizeof(uint32_t));
    if (!arena) {
        perror("Can't allocate arena");
        return 1;
    }

    FILE *log = fopen("measure.log", "w");
    if (!log) {
        free(arena);
        perror("Can't open file");
        return 1;
    }

    /* Forward traverse */

    fprintf(log, "# Forward:\n");
    for (i = 0; i < MAX_DWORDS - 1; ++i) 
        arena[i] = i + 1;

    for (arrSize = MAX_DWORDS; arrSize >= MIN_DWORDS; nextArrSize(&arrSize)) {
        arena[arrSize - 1] = 0;
        printSize(log, arrSize);
        fprintf(log, " %lf\n", traverse(arena, arrSize));
    }

    fflush(log);

    /* Backward traverse */

    fprintf(log, "# Backward:\n");
    for (i = MAX_DWORDS - 1; i > 1; --i)
        arena[i] = i - 1;

    for (arrSize = MAX_DWORDS; arrSize >= MIN_DWORDS; nextArrSize(&arrSize)) {
        arena[0] = arrSize - 1;
        printSize(log, arrSize);
        fprintf(log, " %lf\n", traverse(arena, arrSize));
    }

    fflush(log);

    /* Pseudo-random traverse */

    fprintf(log, "# Pseudo-random:\n");
    bool *freeIdx = (bool*)malloc(MAX_DWORDS * sizeof(bool));
    if (!freeIdx) {
        perror("Can't allocate memory for random traverse");
        free(arena);
        fclose(log);
        return 1;
    }

    puts("Fill done");
    for (i = 0; i < 20; ++i) 
        printf("%u\n", arena[i]);

    for (arrSize = MIN_DWORDS; arrSize <= MAX_DWORDS; arrSize = floor((float)arrSize * 1.2f)) {
        memset(freeIdx, true, MAX_DWORDS);
        freeIdx[0] = false;
        size_t j, nextIdx = 0;
        for (i = 0; i < arrSize - 1; ++i) {
            size_t tmp = bigRand() % arrSize;
            for (j = tmp; (!freeIdx[j % arrSize]) && (j < tmp + arrSize); ++j) {}
            j %= arrSize;
            arena[nextIdx] = j;
            freeIdx[nextIdx] = false;
            nextIdx = j;
        }
        arena[nextIdx] = 0;

        printSize(log, arrSize);
        printSize(stdout, arrSize);
        puts("");
        fprintf(log, " %lf\n", traverse(arena, arrSize));
        fflush(log);
    }

    fclose(log);
    free(arena);
    free(freeIdx);

    return 0;
}
