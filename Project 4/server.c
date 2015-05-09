#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
 
#define BUF_SIZE 1024
#define PORT_NUMBER 80085

//Global Variables
int n, s, port, validationResult, responseCode;
char buf[BUF_SIZE];
struct sockaddr_in self, other;
int len = sizeof(struct sockaddr_in);
char * currentConnectionID = "";
int connectionCount =0;

/*
*	Method		:	validateRequest
*	Purpose		: 
*	Arguments	:
*	Returns		:
*/
int validateRequest(char * request, char * currentConnectionID){
	
	char s[256];
	strcpy(s,request);
	char* token = strtok(s," ");
	int i =1;
	char * command = malloc(sizeof(token));
	char * uniqueID = malloc(sizeof(token));
	char * posInFile = malloc(sizeof(token));
	char * numBytesToRead = malloc(sizeof(token));
	for(i;i<5;i++){
		if(i==1){
		command = token;
		}
		else if(i==2){
		uniqueID = token;
		}
		else if(i==3){
		posInFile = token;
		}
		else if(i==4){
		numBytesToRead = token;
		}
		token = strtok(NULL," ");
	}

	if(strcmp(command,"CONNECT")==0){
		if(connectionCount == 0){
			return 1;
		}
		else{
			return 2;
		}
	}
	else if(strcmp(command,"KILL")==0){
		if(strcmp(currentConnectionID,uniqueID)==0){
			return 3;
		}
		else{
			return 4;
		}
	
	}
	else if(strcmp(command,"DATA")==0){
		if(strcmp(currentConnectionID,uniqueID)==0){
			return 5;
		}
		else{
			return 6;
		}
	}
	else{
		return -1;
	}

}
/*
COMMAND CODES: 
1=connect granted 
2=connect denied 
3=disconnect granted
4=disconnect denied
5=request for data granted
6=request for data denied
*/  
	
/*
*	Method		:	validateRequest
*	Purpose		: 
*	Arguments	:
*	Returns		:
*/
static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
	size_t n=0;
        for (n; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

/*
*	Method		:	validateRequest
*	Purpose		: 
*	Arguments	:
*	Returns		:
*/
char * uniqueID(size_t size)
{
     char *s = malloc(size + 1);
     if (s) {
         rand_string(s, size);
     }
     return s;
}

/*
*	Method		:	validateRequest
*	Purpose		: 
*	Arguments	:
*	Returns		:
*/
int main(int argc, char* argv[]){
	
	//Upon execution, if no second argument is provided, exit and suggest usage. 
    if (argc < 2) {
		fprintf(stderr, "Usage: %s <image file name>\n", argv[0]);
		return 1;
    }
    
    //Copy contents of command argument to new variable filename
	char * filename = malloc(strlen(argv[1] + 1));
	strcpy(filename,argv[1]);
	
	//Attempt to mount floppy
	int isMounted = -1;	
	isMounted = open(filename, O_RDONLY);
	if(isMounted > 0){
		printf("%s mounted successfully! \n",filename);
	}
	if(isMounted < 1){
		printf("Mounting failed. \n");
		return -1;
	}
	
    //Init socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Socket error!");
		return 1;
    }
    
    //Bind to server port
    port = PORT_NUMBER;
    memset((char *) &self, 0, sizeof(struct sockaddr_in));
    self.sin_family = AF_INET;
    self.sin_port = htons(port);
    self.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("%d \n",self.sin_addr.s_addr);
    
    if (bind(s, (struct sockaddr *) &self, sizeof(self)) == -1) {
		perror("Bind error!");
		return 1;
    }
    
    write(1,"Server started, awaiting client request...",sizeof("Server started, awaiting client request..."));
    while ((n = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *) &other, &len)) != -1) {
		printf("Received from %s:%d: ", 
			inet_ntoa(other.sin_addr), 
			ntohs(other.sin_port));

		if((validationResult = validateRequest(buf, currentConnectionID)) == -1){
			perror("Validation error!");
			validationResult = -1;
		}
		
		int commandCode = validationResult;
		
		if(commandCode == 1){
			char * uID;
			uID = uniqueID(10);
			sendto(s,uID,n,0,(struct sockaddr *) &other,strlen(uID));
			connectionCount =1;
			currentConnectionID = uID;
			
			printf("Accepted 'Connect' request from %s:%d: Unique ID is: %s",inet_ntoa(other.sin_addr), ntohs(other.sin_port),currentConnectionID);		
		}
		else if(commandCode == 2){
			char * message = "CONNECTION REFUSED";
			sendto(s,message,n,0,(struct sockaddr *) &other,strlen(message));
			printf("Declined 'Connect' request from %s:%d.",inet_ntoa(other.sin_addr), ntohs(other.sin_port));
		}
		else if(commandCode == 3){
			char * message = "DISCONNECT ACCEPTED";
			sendto(s,message,n,0,(struct sockaddr *) &other,strlen(message));
			connectionCount =0;
			currentConnectionID = "";
			printf("Accepted 'Disconnect' request from %s:%d.",inet_ntoa(other.sin_addr), ntohs(other.sin_port));
		}
		else if(commandCode == 4){
			char * message = "DISCONNECT REFUSED";
			sendto(s,message,n,0,(struct sockaddr *) &other,strlen(message));
			printf("Refused 'Disconnect' request from %s:%d.",inet_ntoa(other.sin_addr), ntohs(other.sin_port));
		}
		else if(commandCode == 5){
			char s[256];
			strcpy(s,buf);
			char* token = strtok(s," ");
			int i =1;
			char * command = malloc(sizeof(token));
			char * uniqueID = malloc(sizeof(token));
			char * posInFile = malloc(sizeof(token));
			char * numBytesToRead = malloc(sizeof(token));
			for(i;i<5;i++){
				if(i==1){
				command = token;
				}
				else if(i==2){
				uniqueID = token;
				}
				else if(i==3){
				posInFile = token;
				}
				else if(i==4){
				numBytesToRead = token;
				}
				token = strtok(NULL," ");
			}
		}
		else if(commandCode == 6){
			char * message = "DATA REFUSED";
			sendto(s,message,n,0,(struct sockaddr *) &other,strlen(message));
			printf("Refused 'Data' request from %s:%d.",inet_ntoa(other.sin_addr), ntohs(other.sin_port));
		}
		else{
			return -1;
		}
	}
	
	fflush(stdout);
	write(1, buf, n);
	write(1, "\n", 1);
	//RESPOND TO CLIENT
	
	sendto(s, buf, n, 0, (struct sockaddr *) &other, len);
    

    close(s);
}

