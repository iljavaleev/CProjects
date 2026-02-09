CC := gcc

all: server client

server: mq_seqnum_server.c  
	$(CC) $^ -o $@

client: mq_seqnum_client.c
	$(CC) $^ -o $@

clean:
	rm -f server client
