CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: server requestget requesthead

server: server.c
	$(CC) $(CFLAGS) -o server server.c

requestget: requestget.c
	$(CC) $(CFLAGS) -o requestget requestget.c

requesthead: requesthead.c
	$(CC) $(CFLAGS) -o requesthead requesthead.c

clean:
	rm -f server requestget requesthead