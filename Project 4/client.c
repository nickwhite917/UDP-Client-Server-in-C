//clienttest.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*
#include <fcntl.h>
#include <ctype.h>
*/

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    int len = sizeof(struct sockaddr_in);
    //char buf[BUF_SIZE];
    struct hostent *host;
    int n, s;
    int port = 80085;
    char * hostString = "0";

    /*if (argc < 0) {
	fprintf(stderr, "Usage: %s <host> <port> <message>\n", argv[0]);
	return 1
    }*/

    host = gethostbyname(argv[1]);
    if (host == NULL) {
	perror("gethostbyname");
	return 1;
    }

    //port = atoi(argv[2]);

    /* initialize socket */
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return 1;
    }
    /* initialize server addr */
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr*) host->h_addr);

	char* buf = "CONNECT 0 0 0";
    /* send message */
    if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &server, len) == -1) {
	perror("sendto()");
	return 1;
    }

    /* receive echo.
    ** for single message, "while" is not necessary. But it allows the client 
    ** to stay in listen mode and thus function as a "server" - allowing it to 
    ** receive message sent from any endpoint.
    */
    while ((n = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *) &server, &len)) != -1) {
    
		printf("Received from %s:%d: ", inet_ntoa(server.sin_addr), ntohs(server.sin_port)); 
		fflush(stdout);
		write(1, buf, n);
		write(1, "\n", 1);
		
    }

    close(s);
    return 0;
}
