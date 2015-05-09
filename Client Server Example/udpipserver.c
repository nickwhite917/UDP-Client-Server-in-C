#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>

void printsin(s_in, s1, s2)
struct sockaddr_in *s_in; char *s1, *s2;
{
	printf ("Program: %s\n%s ",s1,s2);
	printf ("(%d,%d)\n", s_in->sin_addr,s_addr,s_in->sin_port);	
}

main()
{
	short p_len;
	int socket_fd, cc, h_len, fsize, namelen;
	struct sockaddr_in s_in, from;
	struct {char head; u_long body; char tail;} msg;
	socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
	bzero((char *) &s_in, sizeof(s_in));

	s_in.sin_family = (short) AF_INET;
	s_in.sin_addr.s_addr = htonl(INADDR_ANY);
	s_in.sin_port = htons((u_short)0x3333);
	printsin( &s_in, "RECV_UDP","Local socket is: ");
	fflush(stdout);
	bind(socket_fd, (struct socketaddr *)&s_in, sizeof(s_in));
	for(;;){
		fsize = sizeof(from);
		cc = recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr *)&from,&fsize);
		printsin(&from,"recv_udp: ","Packet from: " );
		printf("Got data ::%c%Id%c\n",msg.head, ntohl(msg.body),msg.tail);
		fflush(stdout);
	}
}