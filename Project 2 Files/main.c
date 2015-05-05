//CIS340 PROJECT 2

//William Heeter
//Nelson DiCarlo
//Yu Hang Liu

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "commands.h"

//BEGIN MAIN
int main (int argc, char *arg[]) {

	char *userInput; //what the user types in
	
	size_t buffer = 128; //sets buffer for getline()
	
	char **tokens; //array of pointers to tokens of command typed
	int numTokens; //the number of tokens in input command
	
	tokens = (char **)malloc(sizeof(char *) * 128); //mallocs tokens with a max number of 128
	
//----------------------------------------------------------------------------------------------
//BEGIN PROGRAM WHILE LOOP
	while(1) { 
	
		printf("COMMAND> "); //print prompt to user
		userInput = (char *)malloc(sizeof(char) * 128); //malloc for userInput before tokenizing
		getline(&userInput, &buffer, stdin);// get user input
		userInput[strcspn(userInput, "\n")] = '\0'; //removes null character from end of user input
		
	//---Tokenize the user input
		char *point;
		int count;
		for(count = 0, point = strtok(userInput, " "); point != NULL; count++, point = strtok(NULL, " ")) {
            tokens[count] = (char *)malloc(strlen(point) + 1);
            strcpy(tokens[count], point);
        } //END tokenizing loop
        numTokens = count; //sets the number of token var equal to the number of actual tokens
        
	//---Check for output redirection and handle output if so
        int outputFile, consoleOutput;
        fflush(stdout); //clear stdout as a precaution
        int i; //counter for output redirection loop
        for(i = 0; i < numTokens; i++) {
            if(strcmp(tokens[i], ">") == 0) {
                //Output redirection
                outputFile = open(tokens[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                consoleOutput = dup(STDOUT_FILENO); //Copy original STDOUT

                if(outputFile) {
                    dup2(outputFile, STDOUT_FILENO);
                    close(outputFile);
                }
                break; //break out of output redirection loop
            }
        } //END output redirection for loop
        
	//---QUIT COMMAND - quit the program
		if(strcmp(tokens[0], "quit") == 0) {
			break;
		}//END QUIT COMMAND
		
	//---HELP COMMAND - print the help menu
		else if(strcmp(tokens[0], "help") == 0) {
			printf("\nWelcome to the floppy program!\n");
			printf("Here is a list of commands supported...\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("help - prints the help menu (You just did this!)\n");
			printf("	usage: 	help\n");
			printf("~\n");
			printf("fmount - mounts a floppy image\n");
			printf("	usage:	fmount [floppyname]\n");
			printf("~\n");
			printf("fumount - unmounts the mounted floppy image\n");
			printf("	usage:	fumount\n");
			printf("~\n");
			printf("structure - lists the stucture of the floppy\n");
			printf("	usage:	structure\n");
			printf("~\n");
			printf("traverse - lists the content in the root directory\n");
			printf("	usage:	traverse\n");
			printf("~\n");
			printf("traverse -l - with the -l flag, the traverse command outputs more detailed info\n");
			printf("	usage:	traverse -l\n");
			printf("~\n");
			printf("showsector - show the content (in a hex dump) of the provided sector number\n");
			printf("	usage:	showsector [sectornumber]\n");
			printf("~\n");
			printf("showfat - show the content of the FAT table (in a hex dump)\n");
			printf("	usage: showfat\n");
			printf("~\n");
			printf(">>>THIS PROGRAM SUPPORTS OUTPUT REDIRECTION<<<\n");
			printf("\n");		
		}//END HELP COMMAND
		
	//---FMOUNT COMMAND - mounts the floppy
		else if(strcmp(tokens[0], "fmount") == 0) {	
		
			int mounted;
			
			if(numTokens < 2) {
				printf("ERROR: INCORRECT USAGE - see help menu\n");
				continue;
			}
			else {
				mount(tokens[1]);
				mounted = checkMount();
				if(mounted < 0) {
					printf("ERROR: COULD NOT MOUNT FLOPPY\n");
				}
				else {
				printf("%s is mounted!\n", tokens[1]);
				}
			}				
		} //END FMOUNT COMMAND
		
	//---FUMOUNT FUNCTION - unmounts the floppy
		else if(strcmp(tokens[0], "fumount") == 0) {
			
			int mounted;
			mounted = checkMount();
			
			if(mounted < 0) {
				printf("ERROR: THERE IS NO MOUNTED FLOPPY\n");
				continue;
			}
			else {
				unmount();
				printf("You have unmounted the floppy...\n");	
			}		
		}//END FUMOUNT COMMAND
		
	//---STRUCTURE COMMAND - displays the structure of the floppy
		else if(strcmp(tokens[0], "structure") == 0) {
		
			int mounted;
			mounted = checkMount();
			
			if(mounted < 0) {
				printf("ERROR: THERE IS NO MOUNTED FLOPPY\n");
				continue;
			}
			else {
				structure();
			}	
		}//END STRUCTURE COMMAND
	
	//---TRAVERSE AND TRAVERSE -L COMMANDS
		else if(strcmp(tokens[0], "traverse") == 0) {
			
			int mounted;
			mounted = checkMount();
			
			if(mounted < 0) {
				printf("ERROR: THERE IS NO MOUNTED FLOPPY\n");
				continue;
			}
			else if(strcmp(tokens[1], "-l") == 0) {
				traverse(1);
			}
			else {
				traverse(0);
			}
		} //END TRAVERSE AND TRAVERSE -L COMMANDS
		
	//---SHOWSECTOR COMMAND
		else if(strcmp(tokens[0], "showsector") == 0) {
		
			int mounted;
			mounted = checkMount();
			
			if(mounted < 0) {
				printf("ERROR: THERE IS NO MOUNTED FLOPPY\n");
				continue;
			}
			else if(numTokens < 2) {
				printf("ERROR: INCORRECT USAGE - see help menu\n");
				continue;
			}
			else {
				int sectorNumber = atoi(tokens[1]);
				showsector(sectorNumber);		
			}
		} //END SHOWSECTOR COMMAND
	
	//---SHOWFAT COMMAND
		else if(strcmp(tokens[0], "showfat") == 0) {
		
			int mounted;
			mounted = checkMount();
			
			if(mounted < 0) {
				printf("ERROR: THERE IS NO MOUNTED FLOPPY\n");
				continue;
			}
			else {
				showfat();
			}		
		} //END SHOWFAT COMMAND
			
	//---Put output back to default (the console) (in case output redirection was used)
		fflush(stdout); //clear stdout as a precaution
		if(consoleOutput) {
			dup2(consoleOutput, STDOUT_FILENO);
			close(consoleOutput);
		} //END reset the output to console
	//---Free up userInput and tokens[]
		free(userInput);
		int j;
		for (j = 0; j < numTokens; j++) {
			free(tokens[j]);
		}//END freeing of userInput and tokens[]

	} //END PROGRAM WHILE LOOP

	return 1; //RETURN VALUE FOR main()

} //END MAIN

