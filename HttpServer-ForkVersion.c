#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#define HOST_PORT 9995
#define BUFFER_SIZE 8192

void StartUp(int* socketfd, const short port, struct sockaddr_in* server, int server_size);
void Handle_Request(int socketfd);
 


int main()
{
    int socketfd;
    struct sockaddr_in server, client;
    int sockSize = sizeof(struct sockaddr_in);
    int newSocket, nbyte, pid;

    StartUp(&socketfd, HOST_PORT, &server, sizeof(server));
    signal(SIGCLD, SIG_IGN);

    while(1)
    {
        newSocket = accept(socketfd, (struct sockaddr *)&client, (socklen_t*) &sockSize);
        if(newSocket == -1)
        {
            fprintf(stderr, "Accept Error");
            continue;
        }
        fprintf(stdout, "New socket number is %d\n", newSocket);
        fprintf(stdout, "Accept success!\n");
        pid = fork();
        if(pid < 0)
            exit(3);
        else if(!pid)
        {//child process
            close(socketfd);
            fprintf(stdout, "Handling...\n");
            Handle_Request(newSocket);
            //close(newSocket);
            //exit(EXIT_SUCCESS);

        }
        else
        {//parent process
            close(newSocket);
        }

    }


    return 0;
}


void StartUp(int* socketfd, const short port, struct sockaddr_in* server, int server_size)
{
    int res;
    *socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketfd == -1)
    {
        fprintf(stderr, "Socket create fail...");
        return;
    }
    fprintf(stdout, "Socket create success! Socketfd is %d\n",*socketfd);
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    server->sin_port = htons(port);

    res = bind(*socketfd, (const struct sockaddr* )server, sizeof(*server));
    if(res == -1)
    {
        perror("bind");
        exit(3);
    }
    fprintf(stdout, "Bind success!\n");
    res = listen(*socketfd, 5);
    if(res == -1)
    {
        perror("listen");
        exit(3);
    }
    fprintf(stdout, "Listening...\n");


    return;
}


void Handle_Request(int socketfd)
{
    int i, j, res;
    char buff[BUFFER_SIZE + 1], action[1024], response[BUFFER_SIZE + 1];
    FILE *fptr = NULL;


    res = read(socketfd, buff, sizeof(BUFFER_SIZE));
    for(int i = 0 ; i < strlen(buff) ; i++)
        printf("%c", buff);
    if(res == 0 || res == -1)
    {//Maybe there is a network problem
        fprintf(stdout, "Something get wrong\n");
        exit(3);
    }
    if(strncmp(buff, "GET", 3) && strncmp(buff, "get", 3))
    {//Only handle GET command
        fprintf(stdout, "Only support GET method!\n");
        exit(3);
    }

    for(i = 4, j = 0 ; buff[i] != ' ' ; i++, j++)
    {
        action[j] = buff[i];
    }
    action[j] = '\0';
    printf("action is %s\n", action);
    
    //get index.html
    if(strcmp(action, "/") == 0 || strcmp(action, "/index.html") == 0)
    {
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        write(socketfd, response, strlen(response));
        fptr = fopen("index.html", "r");
        while(!feof(fptr))
        {
            memset(buff, 0, sizeof(buff));
            fread(buff, sizeof(char), sizeof(buff), fptr);
            write(socketfd, buff, sizeof(buff));
        }
    }
    else if(strcmp(action, "/doSomething") == 0)
    {
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Do Something</h1>");
        write(socketfd, response, strlen(response));
    }
    else
    {
        sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        write(socketfd, response, strlen(response));
        fptr = fopen(&action[1], "r");
        while(!feof(fptr))
        {
            memset(buff, 0, sizeof(buff));
            fread(buff, sizeof(char), sizeof(buff), fptr);
            write(socketfd, buff, sizeof(buff));
        }
    }

    return;

}