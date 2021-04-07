#include "func.h"
#include <time.h>

#define IP_MAX_SIZE 15

int mainn(int argc, char *argv[])
{
    srand(time(NULL));

    // char *ip = malloc(IP_MAX_SIZE + 1);
    char *ip;
    short port = 0;
    int num = 0;

    if (argc > 3)
    {
        // strcpy(ip, argv[1]);
        ip = argv[1];
        port = (short)atoi(argv[2]);
        num = atoi(argv[3]);
    }
    else
    {
        printf("You have to specify IP, port and sent value.\n");
        printf("For example:\n");
        printf("./client 127.0.0.1 6875 2\n");
        exit(EXIT_FAILURE);
    }

    CHECK_LAST_CHAR(ip);

    int sock = socket(AF_INET, SOCK_STREAM, 0); // default - TCP
    if (sock == -1)
        HANDLE_ERROR("socket");

    socklen_t size_addr = sizeof(struct sockaddr_in);
    struct sockaddr_in *serv_addr = malloc(size_addr);
    memset(serv_addr, 0, size_addr);

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(port);
    serv_addr->sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)serv_addr, size_addr) == -1)
    {
        // free(ip);
        free(serv_addr);
        close(sock);
        HANDLE_ERROR("connect");
    }

    if (!print_sock_name(sock, NULL))
    {
        // free(ip);
        free(serv_addr);
        close(sock);
        HANDLE_ERROR("getsockname");
    }

    for (int i = 0; i < N; i++)
    {
        printf("Sending %d...\n", num);
        if (send(sock, &num, sizeof(int), 0) <= 0)
        {
            free(serv_addr);
            // free(ip);
            close(sock);
            HANDLE_ERROR("send");
        }
        sleep(num);
    }

    close(sock);
    // free(ip);
    free(serv_addr);
    return 0;
}

