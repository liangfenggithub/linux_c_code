#!/bin/sh
libdir=libwebsockets

gcc -g -o server server.c -I$libdir/include -L$libdir/lib -lwebsockets

# gcc -g -o server server.c -I libwebsockets/include -L libwebsockets/lib -lwebsockets
# gcc -g -o s1 server.c -Ilibwebsockets/include -Llibwebsockets/lib -lwebsockets
# gcc -g -o s2 server2.c -Ilibwebsockets/include -Llibwebsockets/lib -lwebsockets -lpthread
# gcc -g -o s3 server3.c -Ilibwebsockets/include -Llibwebsockets/lib -lwebsockets -lpthread
# gcc -g -o s5 server5.c -Ilibwebsockets/include -Llibwebsockets/lib -lwebsockets -lpthread


