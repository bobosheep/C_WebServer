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
typedef struct FILETYPE FileType;


void HandleRequest(int socketfd);

Method RequestMethod(char* request,const int size);

void HandleAction(int socketfd, Method action, char* request_url);

void WriteHeader(int socketfd, int status, char* contentType);

void WriteContent(int socketfd, char* body, int size);


#endif
