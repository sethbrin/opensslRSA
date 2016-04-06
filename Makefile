CC = gcc
CFLAGS = -O0 -g -I base64/
LDFLAGS = -lcrypto

.PHONY: all

all: rsa rsa_base64

rsa: rsa.c rsacrypto.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

rsa_base64: main.c rsacrypto.h
	$(CC) $(CFLAGS) $(LDFLAGS) base64/lib/libbase64.o $< -o $@

clean:
	rm rsa $(objects)
