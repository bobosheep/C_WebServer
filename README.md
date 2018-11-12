# C_WebServer
A web server implement in C 


## Introduction
This is a project of the class Internet Programming, which use C to implement a web server on the socket connection. In this project, there are two implementation versions by using the fork and the select function to handle connections. 
The fork version server will be listening on port 8888.
The select version server will be listening on port 9999.

## How to use it
*   First, clone this project from github.
```
git clone  https://github.com/bobosheep/C_WebServer.git
```
*   Second, compile the version you want (or both).
```
/**     Compile two version     **/
make 
/**     Compile fork version     **/
make forkVersion
/**     Compile select version     **/
make selectVersion
```
*   Last, run and test.
```
/**     run fork version on port 8888       **/
./HttpServer-ForkVersion
/**     run select version on port 9999     **/
./HttpServer-SelectVersion
```
Then you can open your browser to enjoy the web server.

## Reference
