#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>

int initServer(int &sockServ)
{
    sockaddr_in servAddr;/*Most socket functions require a pointer to a socket address structure as an argument.
 * Each supported protocol suite defines its own socket address structure. The names of these structures begin with
 * sockaddr_ and end with a unique suffix for each protocol suite.

IPv4 Socket Address Structure
An IPv4 socket address structure, commonly called an "Internet socket address structure," is named sockaddr_in and
 is defined by including the <netinet/in.h> header. Figure 3.1 shows the POSIX definition.*/

    socklen_t sockLen = sizeof(servAddr);//socklen_t - an integer type of width of at least 32 bits

    sockServ = socket(AF_INET, SOCK_STREAM, 0);/*AF_INET - Internet domain sockets*//*SOCK_STREAM - Byte-stream socket*/

    servAddr.sin_family = AF_INET;/*IP*/
    servAddr.sin_port = 0;/* bind() will choose a random port*/
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);/*On a server, INADDR_ANY is an argument to bind that tells the socket
                                                * to listen on all available interfaces.*/

    bind(sockServ, (sockaddr*)&servAddr, sockLen);/*bind - bind a name to a socket
 * When a socket is created with socket(2), it exists in a name
       space (address family) but has no address assigned to it.  bind()
       assigns the address specified by addr to the socket referred to
       by the file descriptor sockfd.  addrlen specifies the size, in
       bytes, of the address structure pointed to by addr.
       Traditionally, this operation is called “assigning a name to a
       socket”.

       It is normally necessary to assign a local address using bind()
       before a SOCK_STREAM socket may receive connections (see
       accept(2)).*/

    getsockname(sockServ, (sockaddr*)&servAddr, &sockLen);
    std::cout << "Port: " << htons(servAddr.sin_port) << '\n';
    listen(sockServ, 3);

    return 0;
}

int startTransmition(const int sockCl)
{
    int buf;
    while(true)
    {
        int bitsGot = recv(sockCl, &buf, 4, NULL);
        if (bitsGot == 0)
            break;
        std::cout << buf << '\n';
    }
    exit(0);
    return 0;
}

int getReadyForConnections(const int sockServ)
{
    int sockCl;
    int child;
    sockaddr_in clientAddr;

    socklen_t sockLen = sizeof(clientAddr);

    while(true)
    {
        sockCl = accept(sockServ, (sockaddr*)&clientAddr, &sockLen);
        if ((child = fork()) == 0)
        {
            close(sockServ);
            startTransmition(sockCl);
            close(sockCl);
        }
        else
        {
            close(sockCl);
            int status;
            wait4(child, &status, WNOHANG, NULL);
        }
    }

    return 0;
}

int main()
{
    int sockServ;
    initServer(sockServ);
    getReadyForConnections(sockServ);

    return 0;
}

