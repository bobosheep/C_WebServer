all: HttpServer-ForkVersion.c HttpServer-SelectVersion.c handleHttp.c
	gcc HttpServer-ForkVersion.c handleHttp.c -o HttpServer-ForkVersion
	gcc HttpServer-SelectVersion.c handleHttp.c -o HttpServer-SelectVersion

forkVersion: HttpServer-ForkVersion.c handleHttp.c
	gcc HttpServer-ForkVersion.c handleHttp.c -o HttpServer-ForkVersion

selectVersion: HttpServer-SelectVersion.c	handleHttp.c
	gcc HttpServer-SelectVersion.c handleHttp.c -o HttpServer-SelectVersion

handlehttp: handleHttp.c
	gcc -c handleHttp.c 

