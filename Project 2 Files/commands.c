#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "commands.h"
#include "Fat12Data.h"
#include "Fat12Files.h"


//GLOBAL VARS
char *nameOfFile = '\0'; //pointer for filename
int isMounted = -1; 
struct Fat12Data data;
struct Fat12Files *files;

//------------------------------------------------------------------------------

//MOUNT FUNCTION - open the img and read data from it
void mount(char *filename) {
    nameOfFile = (char *)malloc(strlen(filename) + 1);
    strcpy(nameOfFile, filename);
    isMounted = open(filename, O_RDONLY);
      
    //GET INFO ABOUT FLOPPY
    lseek(isMounted, 11, SEEK_SET); //set the file offset 11 bytes
    
	read(isMounted, &data.BYTES_PER_SECTOR, 2); //get bytes per sector and store it in 'data' struct
	read(isMounted, &data.SECTORS_PER_CLUSTER, 1); //get sectors per cluster and store it in 'data' struct
	read(isMounted, &data.RESERVED_SECTORS, 2); //get reserved sectors (number) and store it in 'data' struct
	read(isMounted, &data.NUM_OF_FATS, 1); //get number of fats and store it in 'data' struct
	read(isMounted, &data.MAX_ROOT_DIRS, 2); //get number or root dirs, store in 'data'
	read(isMounted, &data.TOTAL_SECTORS, 2); //get total number sectors, store in 'data'
	
	lseek(isMounted, 1, SEEK_CUR); //skip 1 byte
	
	read(isMounted, &data.SECTORS_PER_FAT, 2); //get sectors per FAT, store in 'data'
	read(isMounted, &data.SECTORS_PER_TRACK, 2); //get sectors per track, store in 'data'
	read(isMounted, &data.NUM_OF_HEADS, 2); //get number of heads, store in 'data'
	
	lseek(isMounted, 11, SEEK_CUR); //skip 11 bytes
	
	read(isMounted, &data.VOLUME_ID, 4);
	read(isMounted, &data.VOLUME_LABEL, 11);
    
    //GET INFO ABOUT FILES
    files = (struct Fat12Files *)malloc(sizeof(struct Fat12Files) * data.MAX_ROOT_DIRS); //initialize 'files'
    
    lseek(isMounted, ((data.NUM_OF_FATS * data.SECTORS_PER_FAT) + 1) * data.BYTES_PER_SECTOR, SEEK_SET); //move to begining of root directory
    
    int count;
    for(count = 0; count < data.MAX_ROOT_DIRS; count++) {
    	read(isMounted, &files[count].FILENAME, 8); //get FILENAME
    	read(isMounted, &files[count].EXT, 3); //get FILE EXTENSION
    	read(isMounted, &files[count].ATTRIBUTES, 1); //get ATTRIBUTES
    	read(isMounted, &files[count].RESERVED, 2); //RESERVED
    	read(isMounted, &files[count].CREATION_TIME, 2); //get CREATION TIME
    	read(isMounted, &files[count].CREATION_DATE, 2); //get CREATION DATE
    	read(isMounted, &files[count].LAST_ACCESS_DATE, 2); //get LAST ACCESS DATE
    	
    	lseek(isMounted, 2, SEEK_CUR); //skip 2 bytes
    	
    	read(isMounted, &files[count].MODIFY_TIME, 2); //get LAST MODIFIED TIME
    	read(isMounted, &files[count].MODIFY_DATE, 2); //get LAST MODIFIED DATE
    	read(isMounted, &files[count].START_CLUSTER, 2); //get START CLUSTER
    	read(isMounted, &files[count].FILE_SIZE, 4); //get FILE SIZE
    } //END FILE INFO FOR LOOP    
} //END MOUNT FUNCTION

//UNMOUNT FUNCTION
void unmount() {
	close(isMounted);
	isMounted = -1;
}//END UNMOUNT FUNCTION

//STRUCTURE FUNCTION
void structure() {
    printf("        number of FAT:                      %d\n", data.NUM_OF_FATS);
    printf("        number of sectors used by FAT:      %d\n", data.SECTORS_PER_FAT);
    printf("        number of sectors per cluster:      %d\n", data.SECTORS_PER_CLUSTER);
    printf("        number of ROOT Entries:             %d\n", data.MAX_ROOT_DIRS);
    printf("        number of bytes per sector          %d\n", data.BYTES_PER_SECTOR);
    printf("        ---Sector #---      ---Sector Types---\n");
    printf("             0                    BOOT\n");
    
	int i;	
    for(i = 0; i < data.NUM_OF_FATS; i++) {
         printf("          %02d -- %02d                FAT%d\n", (data.SECTORS_PER_FAT * i) + 1, data.SECTORS_PER_FAT * (i + 1), i);
         }
         
    printf("          %02d -- %02d                ROOT DIRECTORY\n", data.SECTORS_PER_FAT * data.NUM_OF_FATS, (data.MAX_ROOT_DIRS / 16) + (data.SECTORS_PER_FAT * data.NUM_OF_FATS));    
} //END STRUCTURE FUNCTION

//TRAVERSE AND TRAVERSE -L FUNCTIONS
void traverse(int option) {
	//TRAVERSE with -L option	
	if(option == 1) {
		printf("    *****************************\n");
        printf("    ** FILE ATTRIBUTE NOTATION **\n");
        printf("    **                         **\n");
        printf("    ** R ------ READ ONLY FILE **\n");
        printf("    ** S ------ SYSTEM FILE    **\n");
        printf("    ** H ------ HIDDEN FILE    **\n");
        printf("    ** A ------ ARCHIVE FILE   **\n");
        printf("    *****************************\n");
        printf("\n");
        
        int q;
		for (q = 0; q < data.MAX_ROOT_DIRS; q++) {
            if (files[q].FILENAME[0] != 0x00 && files[q].START_CLUSTER != 0) {
                char attr[6] = {'-', '-', '-', '-', '-'};
                unsigned char a = files[q].ATTRIBUTES[0];
                if (a == 0x01)
                    attr[0] = 'R';
                if (a == 0x02)
                    attr[1] = 'H';
                if (a == 0x04)
                    attr[2] = 'S';
                if (a == 0x20)
                    attr[5] = 'A';
                if (a == 0x10) {
                	int j;
                    for (j = 0; j < 6; j++)
                        attr[j] = '-';
                }

                if (files[q].ATTRIBUTES[0] == 0x10) {
                    printf("%.6s    %d %d       < DIR >      /%.8s                 %d\n", attr, files[q].CREATION_DATE, files[q].CREATION_TIME, files[q].FILENAME, files[q].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/.                 %d\n", attr, files[q].CREATION_DATE, files[q].CREATION_TIME, files[q].FILENAME, files[q].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/..                 %d\n", attr, files[q].CREATION_DATE, files[q].CREATION_TIME, files[q].FILENAME, 0);
                } else {
                    printf("%.6s    %d %d       %lu      /%.8s.%.3s                 %d\n", attr, files[q].CREATION_DATE, files[q].CREATION_TIME, files[q].FILE_SIZE, files[q].FILENAME, files[q].EXT, files[q].START_CLUSTER);
                }
            }
        }
        
	}//END TRAVERSE with -L option
	//TRAVERSE standard (no -l flag)
	else {
		int q;
		for (q = 0; q < data.MAX_ROOT_DIRS; q++) {
            if (files[q].FILENAME[0] != 0x00 && files[q].START_CLUSTER != 0) {
                if (files[q].ATTRIBUTES[0] == 0x10) {
                    printf("/%.8s                       < DIR >\n", files[q].FILENAME);
                    printf("/%.8s/.                     < DIR >\n", files[q].FILENAME);
                    printf("/%.8s/..                    < DIR >\n", files[q].FILENAME);
                } else {
                    printf("/%.8s.%.3s\n", files[q].FILENAME, files[q].EXT);
                }
            }
        }	
	} //END TRAVERSE standard (no -l flag)
} //END TRAVERSE AND TRAVERSE -L FUNCTIONS

//SHOWSECTOR FUNCTION
void showsector(int sectNum) {
	unsigned char in;

    lseek(isMounted, data.BYTES_PER_SECTOR * sectNum, SEEK_SET);

    printf("        0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\n");
    int q;
    for (q = 0; q < data.BYTES_PER_SECTOR; q++) {
        if (q % 16 == 0 || q == 0) {
            printf("\n");
            printf("%4x", q);
        }
        read(isMounted, &in, 1);
        printf("%5x", in);
    }
    printf("\n");
} //END SHOWSECTOR FUNCTION

//SHOWFAT FUNCTION
void showfat() {
	unsigned char in;
	
    lseek(isMounted, data.BYTES_PER_SECTOR, SEEK_SET);

    printf("        0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\n");
    int q;
    for (q = 0; q < (data.NUM_OF_FATS * data.SECTORS_PER_FAT * data.BYTES_PER_SECTOR); q++) {
        if (q % 16 == 0 || q == 0) {
            printf("\n");
            printf("%4x", q);
        }
        read(isMounted, &in, 1);
        if(in != 0){
        	printf("%5x", in);
        }
        else {
        printf(" FREE");
        }    
    }
    printf("\n");
} //END SHOWFAT FUNCTION
    	
//CHECK MOUNT AND GET FILE DESCRIPTOR
int checkMount() {
	return isMounted;
}

