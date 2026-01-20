CC := gcc

all: server client

server: fifo_seqnum_server.c  
	$(CC) $^ -o $@

client: fifo_seqnum_client.c
	$(CC) $^ -o $@

clean:
	rm -f server client