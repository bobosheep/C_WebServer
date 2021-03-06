#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handleHttp.h"

#define HOST_PORT   8888
#define MAX_LISTEN  20 
#define IP_LENGTH   16


void StartUp(int* socketfd, const short port, struct sockaddr_in* server, int server_size);


int main()
{
    int socketfd, status;
    struct sockaddr_in server, client, *tmp;
    struct in_addr ipAddress;
    socklen_t sockSize;
    int newSocket, nbyte, pid;
    char clientIp[IP_LENGTH];

    StartUp(&socketfd, HOST_PORT, &server, sizeof(server));
    //signal(SIGCLD, SIG_IGN);

    while(1)
    {
        sockSize = sizeof(client);
        newSocket = accept(socketfd, (struct sockaddr *)&client, &sockSize);


        if(newSocket == -1)
        {
            fprintf(stderr, "Accept Error");
            continue;
        }
        //fprintf(stdout, "New socket number is %d\n", newSocket);
        //fprintf(stdout, "Accept success!\n");
        pid = fork();
        if(pid < 0)
            exit(3);
        else if(!pid)
        {//child process

            ipAddress = client.sin_addr;
            inet_ntop(AF_INET, &ipAddress, clientIp, IP_LENGTH);
            printf("[%s] ", clientIp);
            close(socketfd);
            //fprintf(stdout, "Handling...\n");
            HandleRequest(newSocket);
            //close(newSocket);
            exit(EXIT_SUCCESS);

        }
        else
        {//parent process
            close(newSocket);
            wait(&status);
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
    fprintf(stdout, "Listening on port %d...\n", port);


    /**     Finish      **/
    return;
}

