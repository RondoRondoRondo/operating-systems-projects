#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

/*test machine: csel-kh1250-27.cselabs.umn.edu
* group number: G17
* name: Nolan Kelly , Christian Rondestvedt
* x500: kell2490 , ronde021 */
int main(int argc, char** argv){

	if(argc != 3){

		fprintf(stderr,"Usage ./a.out [Path to Directory] [Pattern to search] \n");
		exit(EXIT_FAILURE);
	}
	char* path = argv[1];
	char* pattern = argv[2];

	//Declare any other neccessary variables
	
	struct dirent* entry;
	pid_t pid;
	int nChildren = 0; // initialize number of child processes
	int fd[10][2]; // 10 is the max number of pipes
	char buffer[MAX_READ_LENGTH];
	
	DIR* dir = opendir(path); // open root directory
	// Iterate through root dir and spawn children as neccessary
	//Process each file/directory in the root dir
	//create a child if the file is directory
		//Create a pipe
		//Do I/o redirection for child
		//Exec child
	while((entry = readdir(dir)) != NULL){
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		
		//build path to current entry
		char subpath[MAX_PATH_LENGTH] = "";
		strcat(subpath, path);
		strcat(subpath, "/");
		strcat(subpath, entry->d_name);
		
		//check for soft link
		struct stat* st = (struct stat*)malloc(sizeof(struct stat));
		lstat(subpath, st);
		if(S_ISLNK(st->st_mode)){
			continue;
		}
		free(st);

		//if the current entry is a directory:
		if(entry->d_type==DT_DIR){
			
			//make a pipe for this directory and check to make sure it worked
			if(pipe(fd[nChildren]) == -1){
				fprintf(stderr,"Error: pipe(fd) failed.\n");
			}
			
			//fork and check to make sure fork worked
			pid = fork();
			if(pid==-1){
				fprintf(stderr,"Error: fork() failed.\n"); exit(1);
			}

			//if in parent process:
			if(pid>0){
				//close the write side of pipe and check to make sure it worked
				if(close(fd[nChildren][1]) == -1){
					fprintf(stderr, "Failed to close parent\n");
				}
			}

			//if in child process:
			if(pid==0){
				
				//duplicate file descriptor for STDOUT to the write side of pipe
				if(dup2(fd[nChildren][1],1) == -1){
					fprintf(stderr, "Failed to dup\n");
				}
				//exec child and exit after completes task in order to prevent forking from child
				execl("child","child",subpath,pattern,NULL);
				fprintf(stderr,"Error: exec() failed.\n"); exit(1);
			}
			//mark that a new child process was created
			nChildren += 1;
		}
		
		//if first level entry is a regular file, then no need to create child process and pipe, just skip to pattern search:
		else if(entry->d_type==DT_REG){
			searchPatternInFile(subpath,pattern);
		}
	}

	

	//Wait for all children to complete
	pid_t term_pid;
	for(int i=0;i<nChildren;i++){
		term_pid = wait(NULL);
	}

	//Read pipes of all children and print to stdout
	//Assumption : Pipe never gets full
	for(int i=0;i<nChildren;i++){
		if((read(fd[i][0],buffer,sizeof(buffer)))<0){
			fprintf(stderr,"Error: read() failed.\n");
		}else{
			printf("%s",buffer);
		}
	}
	
	closedir(dir);
	return 0;
}
