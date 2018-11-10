all: HttpServer-ForkVersion.c handleHttp.c
	gcc HttpServer-ForkVersion.c handleHttp.c -o HttpServer-ForkVersion
handlehttp: handleHttp.c
	gcc -c handleHttp.c 