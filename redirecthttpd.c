#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/capsicum.h>

#define BACKLOG 128
#define FORKS 128


void failQuit(char *message) {
    fprintf(stderr, "Error, aborting: %s\n", message);
    _exit(1);
}

int main(int argc, char **argv) {
    char headers[255];
    unsigned int forks=0;
    int s;
    if ((s = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
        failQuit("Unable to open a socket.");

    struct sockaddr_in6 sa;
    bzero(&sa, sizeof sa);
    sa.sin6_family = AF_INET6;
    sa.sin6_port = htons(80);
    sa.sin6_addr = in6addr_any;
    if ((bind(s, (struct sockaddr *)&sa, sizeof sa)) < 0) {
        failQuit("Unable to open port: 80");
    }
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY | SO_REUSEPORT, NULL, 0);
    setsockopt(s, IPPROTO_TCP, SO_LINGER, 0, 1);
    listen(s, BACKLOG);
    unsigned int sz = sizeof(sa);
    int client;
    /* keep from defunctness */
    signal(SIGCHLD, SIG_IGN);
    /* "fork loop" */
    start:;
    forks++;
    /* fork() returns non-zero for the parent, so start over. */
    if (forks <= FORKS) {
        if ( fork() != 0)
            goto start;
    }
    cap_enter();
    /* Request loop */
    while (1) {
        /* accept() blocks until it gets a client. */
        client = accept(s, (struct sockaddr *)&sa, &sz);
        unsigned char len = read(client, headers, sizeof(headers));
        char *host = headers;
        for (int byte=0; byte != len; byte++) {
            if (headers[byte] == ':') {
                host = &headers[byte + 2];
                len -= byte + 2;
                break;
            } 
        }
        for (int byte=0; byte != len; byte++) {
            if (host[byte] == 0x0D || host[byte] == 0x0A) {
                host[byte] = 0;
                break;
            } 
        }
        dprintf(client, "HTTP/1.0 301\r\nLocation: https://%s\r\n", host);
        close(client);
        }
    shutdown(s, SHUT_RDWR);
    close(s);
    return(0);
}
