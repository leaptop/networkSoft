#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int read_from_client(int fields) {
    char buffer;
    int nbytes;

    nbytes = read(fields, &buffer, 1);

    if (nbytes < 0) {
        fprintf(stderr, "read failed\n");
        exit(EXIT_FAILURE);
    } else if (nbytes == 0) {
        return -1;
    }

    fprintf(stdout, "Server: got message: %d\n", buffer);
    return 0;
}

int main(int argc, char **argv) {
    //printf("FD_SETSIZE=%d\n", FD_SETSIZE);//Каждый раз нужно просматривать 1024 сокета. Это многовато. Можно уменьшить.
    int sockfd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    fd_set active_fd_set, read_fd_set;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket failed\n");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = 0;

    if (bind(sockfd, (struct sockaddr *) &addr, addrlen) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(EXIT_FAILURE);
    }

    getsockname(sockfd, (struct sockaddr *) &addr, &addrlen);
    fprintf(stdout, "bound to port: %d\n", addr.sin_port);

    if (listen(sockfd, 3) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fd_set);//обнуляем сет.
    FD_SET(sockfd, &active_fd_set);//Добавляем новое соединение в список подсоединённых клиентов. Добавляем сет. Все сокеты по сути - файловые дескрипторы. Т.е. набор дескрипторов - набор сокетов?

    while (sockfd) {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {//when select returns - we know, that one of our file descriptors has work to do
            fprintf(stderr, "select failed\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {//Ищем дескриптор, который хочет отработать
            if (FD_ISSET (i, &read_fd_set)) {//если ISSET, то понятно, что в этом дескрипторе(i-ом) есть инфа для чтения
                if (i == sockfd) {//Если это серверный сокет, то это - новое соединение, которое надо принять
                    int new;
                    new = accept(sockfd, (struct sockaddr *) &addr, &addrlen);

                    if (new < 0) {
                        fprintf(stderr, "accept failed\n");
                        exit(EXIT_FAILURE);
                    }

                    FD_SET(new, &active_fd_set);//Добавляем новопринятый сокет к сету наблюдаемых сокетов
                } else {//Иначе делаем, что должны с существующим сокетом(читаем из него)
                    if (read_from_client(i) < 0) {
                        close(i);
                        FD_CLR (i, &active_fd_set);//Drop the client
                    }
                }
            }
        }
    }
}