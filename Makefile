CC = gcc
CFLAGS = -O2 -lcrypto -g

rsa: main.c rsacrypto.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm rsa
