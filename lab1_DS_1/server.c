#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void reaper(int signal) {
    wait(0);
    printf("Зомби убит wait-ом!\n");
}

int BuffWork(int sockClient) {
    char buf[1024] = "";
    while (1) {
        int bytes_read = recv(sockClient, buf, 1024, 0);//Системные вызовы recvfrom и recvmsg используются для получения сообщений из сокета, и могут использоваться для получения данных, независимо от того, является ли сокет ориентированным на соединения или нет. Если параметр from не равен NULL, а сокет не является ориентированным на соединения, то адрес отправителя в сообщении не заполняется. Аргумент fromlen передается по ссылке, в начале инициализируется размером буфера, связанного с from, а при возврате из функции содержит действительный размер адреса. Вызов recv обычно используется только на соединенном сокете (см. connect(2)) и идентичен вызову recvfrom с параметром from, установленным в NULL. Все три функции возвращают длину сообщения при успешном завершении.
        if (bytes_read < 0) {
            printf("Плохое получение дочерним процессом.\n");
            exit(1);
        } else if (bytes_read == 0) exit(1);
        printf("Receiving message: %s\n", buf);//Получил сообщение с помощью recv и выдал его в консоль
        printf("Send to client message\n");
        fflush(stdout);
    }
}

int main() {
    signal(SIGCHLD, reaper);//Родительский процесс, если он существует, уведомляется системой о завершении дочерних процессов (или изменении их состояния) сигналом SIGCHLD. Дочерний процесс по завершении становится «зомби» — пустой записью в таблице процессов, хранящей информацию о завершенном процессе (точнее — код завершения) — до тех пор, пока родительский процесс не получит эту информацию с помощью системного вызова wait() или не завершится сам.
    int sockServer, sockClient, child;
    struct sockaddr_in servAddr;//Структура sockaddr_in описывает сокет для работы с протоколами IP. Значение поля sin_family всегда равно AF_INET. Поле sin_port содержит номер порта который намерен занять процесс. Если значение этого поля равно нулю, то операционная система сама выделит свободный номер порта для сокета. Поле sin_addr содержит IP адрес к которому будет привязан сокет. Структура in_addr содержит поле s_addr. Этому полю можно присвоить 32х битное значение IP адреса. Для перевода адреса в целое число из строкового представления можно воспользоваться функцией inet_addr, которой в качестве аргумента передается указатель на строку содержащую IP адрес в виде четырех десятичных чисел разделенных точками. Можно, также, воспользоваться одной из следующих констант:  INADDR_ANY    все адреса локального хоста (0.0.0.0);

    sockServer = socket(AF_INET, SOCK_STREAM, 0);//Создание сокета; socket - создает конечную точку соединения и возвращает ее описатель; SOCK_STREAM    Обеспечивает создание двусторонних надежных и последовательных потоков байтов , поддерживающих соединения. Может также поддерживаться механизм  внепоточных данных.
    if (sockServer < 0) {
        printf("Сервер не может открыть sockServer :(");
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = 0;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);//The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.

    if (bind(sockServer, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {//bind - привязать имя к сокету; int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);  bind привязывает к сокету sockfd локальный адрес my_addr длиной addrlen. Традиционно, эта операция называется lqприсваивание сокету имени.rq Когда сокет только что создан с помощью socket(2), он существует в пространстве имён (семействе адресов), но не имеет назначенного имени.
        printf("Связывание сервера неудачно");
        exit(1);
    }

    socklen_t length = sizeof(servAddr);

    if (getsockname(sockServer, (struct sockaddr *) &servAddr, &length)) { //getsockname  возвращает  текущее  имя  указанного  сокета  в  параметре sockaddr * ( 0 - значит всё правильно, хотя кажется, что имя то должно быть более сложным); Anything that's not a 0 is a true value in if in c language;
        printf("Вызов getsockname неудачен.");
        exit(1);
    }
    printf("Сервер: номер порта - %d\n", ntohs(servAddr.sin_port));
    listen(sockServer, 3);//Сокет начинает прослушиваться сервером; listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept(2).
    while (1) {
        sockClient = accept(sockServer, 0, 0);//accept a connection on a socket; int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);The accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first connection request on the queue of pending(ожидающих) connections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.  The newly created socket is not in the listening state.  The original socket sockfd is unaffected by this call.
        if (sockClient < 0) {//Создание сокета клиентом
            printf("Неверный socket для клиента.\n");
            exit(1);
        }

        child = fork();//In computing, when a process forks, it creates a copy of itself.; More generally, a fork in a multithreading environment means that a thread of execution is duplicated, creating a child thread from the parent thread.
        if (child < 0) {//Начинается создание процессов
            printf("Ошибка при порождении процесса\n");
            exit(1);
        }
        if (child == 0) {
            struct timeval tv;
            tv.tv_sec = 2;
            setsockopt(sockClient, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));//set options on sockets;
            close(sockServer);
            BuffWork(sockClient);
            close(sockClient);
            exit(0);
        }
        close(sockClient);
    }
}
