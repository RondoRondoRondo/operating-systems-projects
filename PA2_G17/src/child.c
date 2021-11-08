#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include<string.h>
#include<stdlib.h>
#include "utils.h"

/*test machine: csel-kh1250-27.cselabs.umn.edu
* group number: G17
* name: Nolan Kelly , Christian Rondestvedt
* x500: kell2490 , ronde021 */
/*
	Traverse the directory recursively and search for pattern in files.
	@params:
		name - path to the directory
		pattern - pattern to be recusrively searched in the directory

	Note: Feel free to modify the function header if neccessary

*/
void dirTraverse(const char *name, char * pattern)
{
	DIR *dir = opendir(name);
	struct dirent *entry;
	
	//Recursively traverse the directory and call SearchForPattern when neccessary
	while((entry = readdir(dir))!=NULL){
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		
		//build path name to current entry
		char fpath[MAX_PATH_LENGTH] = "";
		strcpy(fpath,name);
		strcat(fpath,"/");
		strcat(fpath,entry->d_name);
		
		//check for soft link
		struct stat* st = (struct stat*)malloc(sizeof(struct stat));
		lstat(fpath, st);
		if(S_ISLNK(st->st_mode)){
			continue;
		}
		free(st);
		
		//if entry is reg file, search for pattern
		if(entry->d_type == DT_REG){
			searchPatternInFile(fpath,pattern);
		}
		//if entry is another directory, run again on that directory
		else if(entry->d_type == DT_DIR){
			dirTraverse(fpath,pattern);
		}
	}
	//closedir(dir);
}

int main(int argc, char** argv){

	if(argc !=3){

		fprintf(stderr,"Child process : %d recieved %d arguments, expected 3 \n",getpid(), argc);
		fprintf(stderr,"Usage child.o [Directory Path] [Pattern] \n");
		exit(EXIT_FAILURE);
	}

	char* path = argv[1];
	char* pattern = argv[2];

	close(STDIN_FILENO);

	dirTraverse(path,pattern);

	close(STDOUT_FILENO);

	exit(EXIT_SUCCESS);
}
