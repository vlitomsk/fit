#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @return count if success; -2 if timeout; -1 if another error happened
 */
int read_tout(int fd, void *buf, size_t count, int timeout, int recv_flags, bool readall);
int write_tout(int fd, void *buf, size_t count, int timeout, int send_flags, bool writeall);

#define TIMEOUT 2000
#define FNAME_LENGTH 4096
#define BUF_SZ (FNAME_LENGTH+1)

#define min(a, b) ((a) < (b) ? (a) : (b))

#define DEBUG
#ifdef DEBUG
#define log(...) do { \
    printf("LOG: "); \
    printf(__VA_ARGS__); \
  } while (0);
#else
#define log(...)
#endif

/* Server error codes */

#define CODE_ACK            0

/* supplied file size is too big (>2^62 bytes) */
#define CODE_BAD_FILELEN    4

/* supplied filename length is too big (>4096 bytes) */
#define CODE_BAD_NAMELEN    5

/* supplied filename is dangerous, it contains '/' or '../' or './' */
/* to prevent directory traversal attack */
#define CODE_BAD_FILENAME   6

#define CODE_ERR_TRANSFER   7

#define CODE_ERR_UNKNOWN    8

#define CODE_FILE_EXISTS    9

const char *strservcode(uint8_t code);
void assert_macro(void);

#endif // SERVCODES_H

