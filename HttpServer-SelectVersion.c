#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handleHttp.h"

#define HOST_PORT   9999
#define MAX_LISTEN  20 
#define IP_LENGTH   16


void StartUp(int* socketfd, const short port, struct sockaddr_in* server, int server_size);


int main()
{
    int i, j;
    int socketfd, status;
    int nRead, maxfd, maxidx, cli[__FD_SETSIZE];
    struct sockaddr_in server, client, *tmp;
    struct in_addr ipAddress;
    socklen_t sockSize;
    int newSocket, tmpSocket, nbyte, pid;
    char clientIp[IP_LENGTH];
    fd_set readSet, allSet;


    StartUp(&socketfd, HOST_PORT, &server, sizeof(server));
    //signal(SIGCLD, SIG_IGN);

    maxfd = socketfd;
    maxidx = -1;
    for(i = 0 ; i < __FD_SETSIZE ; i++)
    {
        cli[i] = -1;
    }

    FD_ZERO(&allSet);
    FD_SET(socketfd, &allSet);

    while(1)
    {
        readSet = allSet;
        nRead = select(maxfd + 1, &readSet, NULL, NULL, NULL);

        if(FD_ISSET(socketfd, &readSet))
        {
            sockSize = sizeof(client);
            newSocket = accept(socketfd, (struct sockaddr*) &client, &sockSize);
            
            if(newSocket == -1)
            {
                fprintf(stderr, "Accept Error");
            }
            ipAddress = client.sin_addr;
            inet_ntop(AF_INET, &ipAddress, clientIp, IP_LENGTH);
            printf("[%s] ", clientIp);

            for(i = 0 ; i < __FD_SETSIZE ; i++)
            {
                if(cli[i] < 0)
                {
                    cli[i] = newSocket;
                    break;
                }
            }

            FD_SET(newSocket, &allSet);
            if(newSocket > maxfd)
            {
                maxfd = newSocket;
            }
            if(i > maxidx)
            {
                maxidx = i;
            }

            if(--nRead <= 0)
                continue;
        }

        for(i = 0 ; i <= maxidx ; i++)
        {
            if((tmpSocket = cli[i]) >= 0 )
            {
                if(FD_ISSET(tmpSocket, &readSet))
                {
                    HandleRequest(tmpSocket);

                    close(tmpSocket);
                    FD_CLR(tmpSocket, &allSet);
                    cli[i] = -1;
                    if(--nRead <= 0)
                        break;
                }
            }
        }



    }


    return 0;
}



void StartUp(int* socketfd, const short port, struct sockaddr_in* server, int server_size)
{
    int res;

    /**     Create Socket       **/
    *socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketfd == -1)
    {
        fprintf(stderr, "Socket create fail...");
        return;
    }
    fprintf(stdout, "Socket create success! Socketfd is %d\n",*socketfd);
   
    /**     Socket Setting      **/
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    server->sin_port = htons(port);

    /**     Socket Binding      **/
    res = bind(*socketfd, (struct sockaddr* )server, server_size);
    if(res == -1)
    {
        perror("bind");
        exit(3);
    }
    fprintf(stdout, "Bind success!\n");

    /**     Start Listening Port        **/
    res = listen(*socketfd, MAX_LISTEN);
    if(res == -1)
    {
        perror("listen");
        exit(3);
    }
    fprintf(stdout, "Listening...\n");


    /**     Finish      **/
    return;
}

