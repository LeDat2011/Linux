CC = gcc
CFLAGS = -Wall -pthread
DEPS = 
OBJ = server.o client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server client

server: server.o
	$(CC) -o $@ $^ $(CFLAGS)

client: client.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o server client
