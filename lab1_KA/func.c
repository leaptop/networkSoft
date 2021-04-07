#include "func.h"
#include <sys/wait.h>

void sig_child(int sig)
{
    printf("Child is dead\n");
    wait(NULL);
}

int child(int sock, struct sockaddr_in **host_addr)
{
    int num = 0;
    // if (recv(sock, &num, sizeof(int), 0) <= 0)
    // {
    //     return -1;
    // }
    // printf("%s:%d Recv %d at 1 time\n", inet_ntoa((*host_addr)->sin_addr),
    //                                     ntohs((*host_addr)->sin_port), num);

    for (int i = 1; i <= N; i++)
    {
        if (recv(sock, &num, sizeof(int), 0) <= 0)
        {
            return -1;
        }
        printf("%s:%d Recv value '%d' at %d time\n", inet_ntoa((*host_addr)->sin_addr),
               ntohs((*host_addr)->sin_port), num, i);
    }

    free(*host_addr);
    close(sock);
    exit(EXIT_SUCCESS);
}

int print_sock_name(int sock, struct sockaddr_in **ret)
{
    socklen_t size_addr = sizeof(struct sockaddr_in);
    struct sockaddr_in *tmp = malloc(size_addr);
    memset(tmp, 0, size_addr);

    if (getsockname(sock, (struct sockaddr *)tmp, &size_addr) == -1)
    {
        free(tmp);
        return 0;
    }

    if (size_addr < sizeof(struct sockaddr_in))
        printf("Size is small!\n");
    else
        printf("Socket %s:%d\n", inet_ntoa(tmp->sin_addr), ntohs(tmp->sin_port));

    if (!ret)
        free(tmp);
    else
        *ret = tmp;

    return 1;
}