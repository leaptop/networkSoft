#ifndef __LAB_SOCKET__
#define __LAB_SOCKET__

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


#define HANDLE_ERROR(message) { \
    perror(message);            \
    exit(EXIT_FAILURE);       } \

#define CHECK_LAST_CHAR(str) {      \
    if (str[strlen(str) - 1] == '\n') \
        str[strlen(str) - 1] = 0;     \
                             }      \

#define N 10

void sig_child(int sig);
int print_sock_name(int sock, struct sockaddr_in **ret);
int child(int sock, struct sockaddr_in **host_addr);

#endif