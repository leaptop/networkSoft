#include "func.h"
#include <signal.h>

int main(int argc, char *argv[])
{
    signal(SIGCHLD, sig_child);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    HANDLE_ERROR("socket");

    socklen_t size_addr = sizeof(struct sockaddr_in);
    struct sockaddr_in *serv_addr = malloc(size_addr);
    memset(serv_addr, 0, size_addr);

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = 0;
    serv_addr->sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sock, (struct sockaddr *)serv_addr, size_addr) == -1)
    {
        free(serv_addr);
        close(sock);
        HANDLE_ERROR("bind");
    }

    if (listen(sock, 5) == -1)
    {
        free(serv_addr);
        close(sock);
        HANDLE_ERROR("listen");
    }

    free(serv_addr);

    if (!print_sock_name(sock, NULL))
    {
        close(sock);
        HANDLE_ERROR("getsockname");
    }

    int new_sock = 0, num = 0;
    pid_t pid;
    struct sockaddr_in *host_addr = malloc(size_addr);

    printf("\n");

    while (1)
    {
        num = 0;
        new_sock = accept(sock, (struct sockaddr *)host_addr, &size_addr);
        if (new_sock == -1)
        {
            free(host_addr);
            close(sock);
            HANDLE_ERROR("accept");
        }

        pid = fork();
        if (pid == -1)
        {
            free(host_addr);
            close(sock);
            close(new_sock);
            HANDLE_ERROR("fork");
        }

        if (!pid)
        {
            close(sock);
            if (child(new_sock, &host_addr) == -1)
            {
                free(host_addr);
                close(new_sock);
                HANDLE_ERROR("recv/send");
            }
        }

        close(new_sock);
    }

    free(host_addr);
    close(sock);
    return 0;
}