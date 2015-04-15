#define _BSD_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common.h"

void pservcode(uint8_t code) {
    log("%s\n", strservcode(code));
}

int readcode(int sockfd, int timeout, uint8_t *code) {
    if (read_tout(sockfd, code, sizeof(*code), timeout, 0, false) != sizeof(*code)) {
        log("Error reading servcode\n");
        return -1;
    }
    return 0;
}

int work(int sockfd, int fd, const char *uplname, struct stat st, int timeout) {
    log("uplname: %s\n", uplname);
    assert_macro();
    uint64_t fsize;
    uint8_t servcode;
    ssize_t nbytes;
    char buf[BUF_SZ];

    fsize = htole64((uint64_t) st.st_size);

    log("Sending filesize\n");
    if (write_tout(sockfd, &fsize, sizeof(fsize), timeout, 0, false) != sizeof(fsize)) {
        log("Error sending fsize\n");
        return -1;
    }

    if (readcode(sockfd, timeout, &servcode))
        return -1;

    pservcode(servcode);
    if (servcode)
        return -1;

    ssize_t namelen = min(strlen(uplname), FNAME_LENGTH) + 1;
    memcpy(buf, uplname, namelen);

    log("Sending filename\n");
    if (write_tout(sockfd, buf, namelen, timeout, 0, false) != namelen) {
        log("Error sending filename\n");
        return -1;
    }

    if (readcode(sockfd, timeout, &servcode))
        return -1;

    pservcode(servcode);
    if (servcode)
        return -1;

    log("Sending file\n");
    nbytes = 0;
    while ((uint64_t)nbytes < fsize) {
        fsize -= nbytes;
        nbytes = read(fd, buf, min(BUF_SZ, fsize));
        if (nbytes == -1) {
            perror("read()");
            return -1;
        }
        if ((nbytes = write_tout(sockfd, buf, min(BUF_SZ, fsize), timeout, 0, false)) < 0) {
            log("Sending file error\n");
            return -1;
        }
    }

    if (readcode(sockfd, timeout, &servcode))
        return -1;

    pservcode(servcode);
    if (servcode)
        return -1;

    return 0;
}

int main(int argc, char **argv) {
    struct addrinfo *servinfo, aihints;
    int sockfd, gaist, fd;
    char *arghost, *argport, *argpath, *uplname;
    struct stat st;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s host port filename\n", argv[0]);
        return 1;
    }

    arghost = argv[1];
    argport = argv[2];
    argpath = argv[3];

    uplname = strrchr(argpath, '/');
    if (!uplname)
        uplname = argpath;
    else
        ++uplname;


    fd = open(argpath, O_RDONLY);
    if (fd == -1) {
        perror("open()");
        return 1;
    }
    fstat(fd, &st);
    if (S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Sending directories is not supported\n");
        return 1;
    }

    memset(&aihints, 0, sizeof(struct addrinfo));
    aihints.ai_flags |= AI_NUMERICSERV;
    aihints.ai_family = AF_INET;
    gaist = getaddrinfo(arghost, argport, &aihints, &servinfo);
    if (gaist) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(gaist));
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket()");
        return 1;
    }

    if (connect(sockfd, servinfo->ai_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("connect()");
        return 1;
    }

    work(sockfd, fd, uplname, st, TIMEOUT);

    freeaddrinfo(servinfo);
    close(sockfd);
    close(fd);

    return 0;
}
