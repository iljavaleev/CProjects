CC := gcc

all: server client1 client2

server: server.c  
	$(CC) $^ -o $@

client1: first_client.c
	$(CC) $^ -o $@

client2: second_client.c
	$(CC) $^ -o $@

clean:
	rm -f server client1 client2