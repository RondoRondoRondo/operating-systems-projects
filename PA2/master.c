#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"


int main(int argc, char** argv){

	if(argc != 3){

		fprintf(stderr,"Usage ./a.out [Path to Directory] [Pattern to search] \n");
		exit(EXIT_FAILURE);
	}
	char* path = argv[1];
	char* pattern = argv[2];

	//Declare any other neccessary variables
	//Open root directory
	struct dirent* entry;
	struct stat buf;
	pid_t pid;
	int nChildren = 0; // initialize number of child processes
	int fds[10][2]; // 10 is the max number of pipes
	DIR* dir = opendir(path); // open root directory

	// Iterate through root dir and spawn children as neccessary
	//Process each file/directory in the root dir
	//create a child if the file is directory
		//Create a pipe
		//Do I/o redirection for child
		//Exec child
	while((entry=readdir(dir)!=NULL)){
		if(!strcmp(entry->d_name,".") || !strcmp(entry->d_name,"..")) continue;
		if(entry->d_type==DT_DIR){
			nChildren += 1;
			pid = fork();
			if(pid==-1){
				fprintf(stderr,"Error: fork() failed.\n"); exit(1);
			}
			if(pid==0){
				char childPath[100];
				strcpy(childPath,path);
				strcat(childPath,"/");
				strcat(childPath,entry->d_name);
				execl("child","child",childPath,pattern,NULL);
				fprintf(stderr,"Error: exec() failed.\n"); exit(1);
			}
		}
		else{
			
		}
	}
for(int i=0;i<nChildren,i++){
	wait(NULL);
}
	//Wait for all children to complete
	//Read pipes of all children and print to stdout
	//Assumption : Pipe never gets full

	return 0;
}
