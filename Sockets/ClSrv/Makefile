CC := gcc

all: server client

server: socket_seqnum_server.c  
	$(CC) $^ -o $@

client: socket_seqnum_client.c
	$(CC) $^ -o $@

clean:
	rm -f server client