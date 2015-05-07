/***********************************************************************\
* 			   SEND_UDP.C				        *
* Test of UDP/IP. Send a single dippy datagram to a receiver process    *
* assumed to exist on port 0x3333 on the internet host specified as     *
* the single argument to this program.				        *
* 								        *
* To use:							        *
* 	1) Make sure that recv_udp is running (probably in the	        *
* 	   background on the target host.			        *
* 	2) Issue the command "send_udp xx", where xx is a host name.    *
* 								        *
* Phil Kearns							        *
* April 11, 1987						        *
* 								        *
\***********************************************************************/

#include	<signal.h>
#include	<errno.h>
#include	<strings.h>
#include 	<stdlib.h>
#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>

int
main(argc,argv)
char **argv;

{
    int	socket_fd, cc;
    long getpid();
    struct sockaddr_in	dest;
    struct hostent *gethostbyname(), *hostptr;
    
    struct {
	char	head;
	u_long	body;
	char	tail;
    } msgbuf;

/*
   Set up a datagram (UDP/IP) socket in the Internet domain.
   We will use it as the handle thru which we will send a
   single datagram. Note that, since this no message is ever
   addressed TO this socket, we do not bind an internet address
   to it. It will be assigned a (temporary) address when we send
   a message thru it.
*/

    socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
	perror ("send_udp:socket");
	exit(1);
    }

/*
   The inet socket address data structure "dest" will be used to
   specify the destination of the datagram. We must fill in the
   blanks.
*/

    bzero((char *) &dest, sizeof(dest)); /* They say you must do this */
    if ((hostptr = gethostbyname(argv[1])) == NULL){
	fprintf(stderr, "send_udp: invalid host name, %s\n",argv[1]);
	exit(1);
    }
    dest.sin_family = AF_INET;
    bcopy(hostptr->h_addr, (char *)&dest.sin_addr,hostptr->h_length);
    dest.sin_port = htons((u_short)0x3333);

    msgbuf.head = '<';
    msgbuf.body = htonl(getpid());
    msgbuf.tail = '>';

    cc = sendto(socket_fd,&msgbuf,sizeof(msgbuf),0,(struct sockaddr *)&dest,
                  sizeof(dest));
    if (cc < 0) {
	perror("send_udp:sendto");
        exit(1);
    }

    return(0);
}



