#include<stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include "utils.h"

/*test machine: csel-kh1250-27.cselabs.umn.edu
* group number: G17
* name: Nolan Kelly , Christian Rondestvedt
* x500: kell2490 , ronde021 */

/*
	Searches for pattern in file and writes the entire line containing the pattern to STDOUT
	Format for print: [path to file] : Line in the file
	@Params:
		path - path to the file
		pattern : pattern to be searched
*/
void searchPatternInFile(char* path, char* pattern){

	
	FILE * fp;

	fp = fopen(path, "r");
	if (fp == NULL){
		fprintf(stderr,"Error opening file: %s \n",path);
		return;
	}
	
	//Read file line by line and check if pattern exists in the line
	char buffer[MAX_READ_LENGTH];
	while(fgets(buffer, sizeof(buffer), fp) != NULL){
		if(strstr(buffer,pattern) != NULL){
			//build correct output to be written to STDOUT
			char writebuf[MAX_WRITE_LENGTH];
			strcpy(writebuf,path);
			strcat(writebuf," : ");
			strcat(writebuf,buffer);
			write(STDOUT_FILENO, writebuf, strlen(writebuf));
		}
	}

		
	//Close fp
	fclose(fp);
}
