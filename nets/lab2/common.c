#include "common.h"
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include <stdio.h>
#include <assert.h>

#define RWOP_READ 0
#define RWOP_WRITE 1

static char* codes[256] = {
    "Success", 0, 0, 0,
    "Bad file length",
    "Bad filename length",
    "Bad filename (may be containing slashes)",
    "Error while transferring file",
    "Unknown server error",
    "File exists", 0, 0, 0, 0, 0, 0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const char *strservcode(uint8_t code) {
    return codes[code];
}

int rw_tout(int fd, void *buf, size_t count, int timeout, int rs_flags, bool rwall, short polldir) {
    ssize_t n, rwcount = 0;
    struct pollfd pfd;
    int pres;

    pfd.fd = fd;
    while (rwcount < count) {
        pfd.events = polldir;
        pfd.revents = 0;
        if ((pres = poll(&pfd, 1, timeout)) == -1) {
            perror("poll() in rw_tout");
            return -1;
        }

        if (pres == 0) {
            log("Time out!\n");
            return -2;
        }

        if (pfd.revents & polldir) {
            if ((polldir == POLLIN && (n = recv(fd, buf, count - rwcount, rs_flags | MSG_DONTWAIT)) == -1) ||
                (polldir == POLLOUT && (n = send(fd, buf, count - rwcount, rs_flags | MSG_DONTWAIT)) == -1))
            {
                perror("send()/recv() in rw_tout");
                return -1;
            }
            rwcount += n;
        }

        if (!rwall)
            break;
    }

    return rwcount;
}

int read_tout(int fd, void *buf, size_t count, int timeout, int recv_flags, bool readall) {
    return rw_tout(fd, buf, count, timeout, recv_flags, readall, POLLIN);
}

int write_tout(int fd, void *buf, size_t count, int timeout, int send_flags, bool writeall) {
    return rw_tout(fd, buf, count, timeout, send_flags, writeall, POLLOUT);
}

void assert_macro(void) {
    assert(BUF_SZ >= FNAME_LENGTH);
}
