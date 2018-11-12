#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "handleHttp.h"

/**     DEFINITION      **/
#define BUFFER_SIZE 4096


/**     TYPE DEFINITION      **/
enum METHOD
{
    ERROR = 0,
    GET = 1,
    POST
} ;

struct FILETYPE
{
    char* filetype;
    char* contentType;
}; 


/**     GLOBAL VARIABLE      **/
FileType extFileType [] =   {   {"html", "text/html"}, {"htm", "text/html"},\
                                {"js", "application/javascript"}, {"css", "text/css"}, \
                                {"jpeg", "image/jpeg"}, {"jpg", "image/jpeg"}, \
                                {"gif", "image/gif"}, {"png", "image/png"}, \
                                {"zip", "image/zip"}, {"gz", "image/gz"},
                                {0, 0}
                            };


void HandleRequest(int socketfd)
{
    int i, j, res, bufSize, content_length;
    Method method;
    char buff[BUFFER_SIZE + 1], action[1024], response[BUFFER_SIZE + 1], dataInfo[BUFFER_SIZE + 1];


    res = read(socketfd, buff, BUFFER_SIZE);
    //printf("res = %d\n", res);
    //printf("%s\n", buff);

    method = RequestMethod(buff, res);
    content_length = GetContentLength(buff, res);

    switch(method)
    {
        case 0:
            printf("Undefine Method");
            break;
        case 1:
            printf("GET ");
            break;
        case 2:
            printf("POST ");
            break;
    }

    for(i = method == GET ? 4 : 5, j = 0 ; buff[i] != ' ' ; i++, j++)
    {
        action[j] = buff[i];
    }
    action[j] = '\0';
    printf("%s\n", action);

    memcpy(dataInfo, &buff[res - content_length], content_length);
    dataInfo[content_length] = '\0';
    printf("data info is %s\n", dataInfo);

    HandleAction(socketfd, method, action, dataInfo);


    return;

}

Method RequestMethod(char* request,const int size)
{
    Method method;
    int i;

    if(size == 0 || size == -1)
    {//Maybe there is a network problem
        method = ERROR;
        return method;
    }

    if(size > 0 && size < BUFFER_SIZE)
        request[size] = 0;
    else request[0] = 0;
    

    if(strstr(request, "GET ") != NULL || strstr(request, "get ") != NULL)
    {
        return method = GET;
    }
    else if(strstr(request, "POST ") != NULL || strstr(request, "post ") != NULL)
    {
        return method = POST;
    }
    else return method = ERROR;


}

int GetContentLength(char* request, const int size)
{
    int content_length = 0;
    char* input = NULL;
    //printf("get Content length!\n");
    input = strstr(request, "Content-Length:");
    //printf("request is  %s\n", request);
    //printf("input is  %s\n", input);
    if(input != NULL)
    {
        sscanf(input, "Content-Length: %d", &content_length);
        //printf("length is %d", content_length);
    }
    return content_length;
}


void HandleAction(int socketfd, Method action, char* request_url, char* dataInfo)
{
    char buff[BUFFER_SIZE];
    char* ftype;
    Method Get = GET, Post = POST;
    int i;
    FILE *fptr = NULL;
    
    if(action == GET)
    {        
        /**     /index.html        **/
        if(strcmp(request_url, "/") == 0 || strcmp(request_url, "/index.html") == 0)
        {
            WriteHeader(socketfd, 200, "text/html");
            fptr = fopen("index.html", "r");
            while(!feof(fptr))
            {
                memset(buff, 0, sizeof(buff));
                fread(buff, sizeof(char), sizeof(buff), fptr);
                WriteContent(socketfd, buff, sizeof(buff));
            }
        }

        /**     /dosomething        **/
        else if(strcmp(request_url, "/dosomething") == 0)
        {
            WriteHeader(socketfd, 200, "text/html");
            WriteContent(socketfd, "<h1>Just Do Something</h1>", 26);
        }


        else if(strcmp(request_url, "/login") == 0)
        {
            WriteHeader(socketfd, 200, "text/html");
            fptr = fopen("login.html", "r");
            while(!feof(fptr))
            {
                memset(buff, 0, sizeof(buff));
                fread(buff, sizeof(char), sizeof(buff), fptr);
                WriteContent(socketfd, buff, sizeof(buff));
            }
        }

        /**     /<filename>        **/
        else
        {
            short isJs = 0;
            for(i = 0 ; extFileType[i].filetype != 0 ; i++)
            {
                int len = strlen(extFileType[i].filetype);
                int requestlen = strlen(request_url);
                if(!strncmp(request_url + requestlen - len, extFileType[i].filetype, len))
                {
                    isJs = i == 2 ? 1 : 0;      ///check if is a javascript file
                    ftype = extFileType[i].contentType;
                    break;
                }
            }
            if(ftype == 0)
            {// 404 Not Found or maybe is not support file type
                WriteHeader(socketfd, 404, "text/html");
                WriteContent(socketfd, "<h1>404 Not Found</h1>", 22);
            }
            
            else
            {
                fptr = fopen(&request_url[1], "r");
                if(fptr == NULL)
                {//Can't not open the file
                    WriteHeader(socketfd, 404, "text/html");
                    WriteContent(socketfd, "<h1>404 Not Found</h1>", 22);
                }
                else
                {
                    WriteHeader(socketfd, 200, ftype);
                    while(!feof(fptr))
                    {
                        memset(buff, 0, sizeof(buff));
                        fread(buff, sizeof(char), sizeof(buff), fptr);
                        //printf("%d\n", sizeof(buff));
                        if(isJs)
                            WriteContent(socketfd, buff, strlen(buff));
                        else WriteContent(socketfd, buff, sizeof(buff));
                    }
                }
            }

        }
    }
    else if(action == POST)
    {
        char* usr, *password, *ptr, response[1024];
        if(strcmp(request_url, "/login") == 0)
        {
            WriteHeader(socketfd, 200, "text/html");
            usr = strstr(dataInfo, "username=");
            password = strstr(dataInfo, "password=");
            ptr = strstr(dataInfo, "&");
            *ptr = '\0';
            sprintf(response, "<h1>Welcome %s !</h1>", &usr[9]);
            WriteContent(socketfd, response, strlen(response));
        }
    }
    return ;
}

void WriteHeader(int socketfd, int status, char* contentType)
{
    char response[4096];
    switch(status)
    {
        case 200:
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", contentType);
            break;
        case 400:
            sprintf(response, "HTTP/1.1 400 Bad Request\r\nConnection: Keep-Alive\r\nContent-Type: %s\r\n", contentType);
            break;
        case 404:
            sprintf(response, "HTTP/1.1 404 Not Found\r\nConnection: Keep-Alive\r\nContent-Type: %s\r\n", contentType);
            break;
    }
    write(socketfd, response, strlen(response));
}

void WriteContent(int socketfd, char* body, int size)
{
    write(socketfd, body, size);
}