CC=gcc
CFLAGS=-g -m32 -Wall

all: recv_udp send_udp fancy_recv_udp

recv_udp: recv_udp.o
	$(CC) -m32 -o recv_udp recv_udp.o

fancy_recv_udp: fancy_recv_udp.o
	$(CC) -m32 -o fancy_recv_udp fancy_recv_udp.o

send_udp: send_udp.o
	$(CC) -m32 -o send_udp send_udp.o

clean:
	rm -f *.o recv_udp send_udp fancy_recv_udp
