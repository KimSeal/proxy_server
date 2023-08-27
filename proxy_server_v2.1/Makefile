CC= gcc

all: server client

server: server.c
	$(CC) -o $@ $< -lcrypto

client: client.c
	$(CC) -o $@ $< -lcrypto
