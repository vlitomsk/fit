#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/mman.h>

const uint32_t instrCacheSize = 32768; // 32K
const uint32_t maxAssoc = 12;
const uint32_t passCount = 5000;

inline void placeJmp(uint8_t *addrFrom, const uint8_t *addrTo) {
  addrFrom[0] = 0xe9;
  *(uint32_t*)(addrFrom + 1) = ((uint32_t)addrTo - (uint32_t)addrFrom - 5); // 5 -- jmp far size
}

// http://pdos.csail.mit.edu/6.828/2008/readings/i386/Jcc.htm
inline void placeJnz(uint8_t *addrFrom, const uint8_t *addrTo) {
  addrFrom[0] = 0x0f;
  addrFrom[1] = 0x85;
  *(uint32_t*)(addrFrom + 2) = ((uint32_t)addrTo - (uint32_t)addrFrom - 6); 
}

double callFunction(uint8_t *fn) {
  union ticks {
    uint64_t t64;
    struct s32 { uint32_t th, tl; } t32;
  } start, end;

  void (*fptr)(void) = (void(*)(void))(fn);
  asm("pushal\n"
      "movl %0, %%ecx" : : "r" (passCount));

  fptr();

  asm("movl %%edx, %0" : "=m" (end.t32.tl));
  asm("movl %%eax, %0" : "=m" (end.t32.th));
  asm("movl %%ecx, %0" : "=m" (start.t32.tl));
  asm("movl %%ebx, %0" : "=m" (start.t32.th));
 
  asm("popal");
  
  return end.t64 - start.t64;
}

int main(int argc, char **argv) {
  uint32_t testAssoc, i;
  int pageSize = sysconf(_SC_PAGE_SIZE);
  if (pageSize == -1) {
    perror("Can't get page size");
    return EXIT_FAILURE;
  }
  uint8_t *arrNotAligned = (uint8_t*)malloc(instrCacheSize * maxAssoc + pageSize);
  uint8_t *arr = (uint8_t*)(((uint32_t)arrNotAligned + pageSize - 1) & ~(pageSize - 1));
  
  if (!arrNotAligned) {
    perror("Can't allocate memory");
    return EXIT_FAILURE;
  }

  if (mprotect(arr, instrCacheSize * maxAssoc, PROT_EXEC | PROT_WRITE | PROT_READ)) {
    perror("Can't set protection");
    free(arrNotAligned);
    return EXIT_FAILURE;
  }

  printf("mprotect ok! aligned addr: %p; not aligned addr: %p\n", arr, arrNotAligned);
  memset(arrNotAligned, 0x90, instrCacheSize * maxAssoc + pageSize);

  FILE* log = fopen("measure.log", "w");
  if (!log) {
    perror("Can't open file");
    free(arr);
    return EXIT_FAILURE; 
  }

  for (testAssoc = maxAssoc; testAssoc >= 2; --testAssoc) {
    printf("\nTrying assoc %d\n", testAssoc);
    for (i = 0; i < testAssoc; ++i) {
      printf("  address of %d piece: %p\n", i, arr + instrCacheSize * i); 
      if (i == 0) {
        // first piece
        arr[0] = 0x0f; // rdtsc
        arr[1] = 0x31;
        placeJmp(arr + 2, arr + instrCacheSize);
        continue;
      }

      if (i == testAssoc - 1) {
        // last piece
        uint32_t offs = i * instrCacheSize;
        uint8_t bytecode[] = { 
          0x49, // dec ecx
          0x85, 0xc9, // test ecx, ecx
          0, 0, 0, 0, 0, 0, // jnz FIRST_PIECE
          0x89, 0xc3, // mov ebx, eax 
          0x89, 0xd1, // mov ecx, edx
          0x0f, 0x31, // rdtsc
          0xc3 // ret
        };
        memcpy(arr + offs, bytecode, sizeof(bytecode));
        placeJnz(arr + offs + 3, arr + 2);
        continue;
      }

      placeJmp(arr + i * instrCacheSize,
              arr + (i + 1) * instrCacheSize);
    }
    printf("Assoc %d filled! Testing now..\n", testAssoc);

    double measuredTime = callFunction(arr) / (double)(testAssoc * passCount);
    fprintf(log, "Assoc: %d ; Time: %lf\n", testAssoc, measuredTime);
    fflush(log);
    printf("Done\n");
  }

  fclose(log);
  free(arrNotAligned);
  return EXIT_SUCCESS;
}

