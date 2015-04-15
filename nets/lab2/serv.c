#define _BSD_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <endian.h>
#include <poll.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "common.h"

/* uploads directory path with trailing slash */
#define DIRNAME "uploads/"
#define DIRLEN 8

bool serv_working = true;

inline int send_code(int sockfd, uint8_t code, int timeout) {
    log("Sending code: %d (%s)\n", code, strservcode(code));
    if (write_tout(sockfd, &code, sizeof(code), timeout, 0, false) != sizeof(code)) {
        log("Error while sending code; Closing socket\n");
        return -1;
    }
    return 0;
}

inline bool check_fname(char *filename) {
    return !strchr(filename, '/') && !strchr(filename, '\\');
}

/**
 * PROTOCOL DESCRIPTION
 * All sent numbers are in little-endian.
 * All server codes defined in 'servcodes.h'
 * When server sends not CODE_ACK, it brokes connection.
 * 1. Client (C) connects to server (S)
 * 2. C>S : [uint64_t filesize]
 * 3. C<S : [uint8_t ret code]
 * 4. C>S : [char *filename]
 * 5. C<S : [uint8_t ret code]
 * 6. C>S : [char *file_bytes]
 * 7. C<S : [uint8_t ret code]
 */

ssize_t fname_len(const char *filename, ssize_t nbytes) {
    ssize_t i;
    for (i = 0; i < nbytes; ++i) {
        if (filename[i] == '\0')
            return i;
    }
    return -1; // non 0-terminated string
}

int handle_client(int csockfd, int timeout) {
    uint64_t fsize;
    ssize_t namelen;
    char buf[BUF_SZ];
    char path[DIRLEN + FNAME_LENGTH + 1];
    char *filename = path + DIRLEN;
    ssize_t nbytes;
    bool delete_file;

    /* reading filesize */
    nbytes = read_tout(csockfd, &fsize, sizeof(fsize), timeout, 0, false);
    if (nbytes != sizeof(fsize)) {
        log("Filesize reading error\n");
        return -1;
    }
    fsize = le64toh(fsize);

    if (fsize > ((uint64_t)1 << 62)) {
        log("Filesize is too big\n");
        send_code(csockfd, CODE_BAD_FILELEN, timeout);
        return -1;
    }

    if (send_code(csockfd, CODE_ACK, timeout))
        return -1;
    log("Filesize %llu accepted\n", fsize);

    /* reading filename */
    //memset(path, 0, sizeof(path));
    memcpy(path, DIRNAME, DIRLEN);

    nbytes = read_tout(csockfd, buf, sizeof(buf), timeout, MSG_PEEK, false);
    if (nbytes > 0) {
        namelen = fname_len(buf, nbytes);
        if (namelen <= 0 || namelen > FNAME_LENGTH) {
            log("Bad filename length\n");
            send_code(csockfd, CODE_BAD_FILELEN, timeout);
            return -1;
        }
        /* There is no lseek() for socket, so .. */
        read_tout(csockfd, filename, namelen + 1, 0, 0, true);
    } else {
        log("Filename reading error");
        send_code(csockfd, CODE_ERR_UNKNOWN, timeout);
        return -1;
    }

    if (!check_fname(filename)) {
        log("Bad filename (check_fname)!\n");
        send_code(csockfd, CODE_BAD_FILENAME, timeout);
        return -1;
    }

    if (access(path, F_OK) != -1) {
        send_code(csockfd, CODE_FILE_EXISTS, timeout);
        return -1;
    }
    int fd = open(path, O_CREAT | O_WRONLY, 0600);
    if (fd == -1) {
        perror("open()");
        if (errno == ENAMETOOLONG)
            send_code(csockfd, CODE_BAD_FILELEN, timeout);
        else
            send_code(csockfd, CODE_ERR_UNKNOWN, timeout);
        return -1;
    }

    if (send_code(csockfd, CODE_ACK, timeout))
        return -1;
    log("Filename `%s` accepted and file exchange started\n", filename);

    /* reading file */
    nbytes = 0;
    delete_file = false;

    do {
        fsize -= (uint64_t) nbytes;
        nbytes = read_tout(csockfd, buf, sizeof(buf), timeout, 0, false);
        if (nbytes > 0) {
            if (write(fd, buf, min(nbytes, fsize)) == -1) {
                perror("write()");
                send_code(csockfd, CODE_ERR_TRANSFER, timeout);
                delete_file = true;
                break;
            }
        }
    } while (nbytes > 0 && nbytes < fsize);

    if (nbytes < 0) {
        log("Error while transfer\n");
        send_code(csockfd, CODE_ERR_TRANSFER, timeout);
        delete_file = true;
    }

    close(fd);

    if (delete_file) {
        log("Deleting file\n");
        unlink(path);
        return -1;
    }

    if (send_code(csockfd, CODE_ACK, timeout))
        log("Warning: send_code was unsuccessful, but file recieved ok");
    log("File `%s` recieved ok!\n", path);

    return 0;
}

int main( int argc, char *argv[] )
{
    assert_macro();
    assert(DIRLEN == strlen(DIRNAME));

    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct timeval send_tout = { .tv_sec = 2, .tv_usec = 0 },
                   recv_tout = { .tv_sec = 2, .tv_usec = 0 };

    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        perror("socket()");
        return 1;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind()");
        return 1;
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while (serv_working) {
        log("waiting for client\n");
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd == -1) {
            perror("accept()");
            continue;
        }

        if (fcntl(newsockfd, F_SETFL, O_NONBLOCK) == -1) {
            perror("fcntl() O_NONBLOCK");
            close(newsockfd);
            continue;
        }

        log("Handling new client\n");
        handle_client(newsockfd, TIMEOUT);
        log("Closing connection with client\n");
        close(newsockfd);
    }

    close(sockfd);

    return 0;
}
