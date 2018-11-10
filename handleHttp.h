#ifndef HANDLEHTTP_H
#define HANDLEHTTP_H

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

typedef enum METHOD Method;
// error = 0, get = 1, post = 2

typedef struct FILETYPE FileType;
//supported file type

void HandleRequest(int socketfd);
//give a socket file descrtiptor, then the function will handle the request message

Method RequestMethod(char* request,const int size);
//Get the request method, only support GET and POST currently

void HandleAction(int socketfd, Method action, char* request_url);
//Do the request action

void WriteHeader(int socketfd, int status, char* contentType);
//To write the Header of the response

void WriteContent(int socketfd, char* body, int size);
//To write the Content of the response


#endif
