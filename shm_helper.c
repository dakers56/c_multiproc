#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <error.h>

#include "shm_helper.h"

/*
int main(int argc, char **argv){
	printf("Testing shared memory helper.\n");
	if (argc != 2){
		printf("Must provide exactly one path.\n"); 
		return 0;
	}
	int res = helper_open_default(argv[1], 24);
	if(res == -1) return -1;
	res = helper_unlink(argv[1]);
	return res == 0;	
	
	
}
*/



int helper_open(char *path, int oflag,  mode_t mode, int size){
	int shmfd = shm_open(path, oflag, mode);
        if (shmfd == -1){
	printf("Shm helper could not open shared memory woth path %s.\n", path);
        return -1;
        }
        int fail  = ftruncate(shmfd, size);
	if(fail){
		perror("Failed to truncate file.");
		return -1;
	}
        printf("Created shared memory object with FD %d.\n", shmfd);
	return shmfd;
}

int helper_open_default(char *path, int size){
	return helper_open(path, O_CREAT| O_RDWR, 0666, size);
}

int helper_unlink(char *path){
	printf("Unlinking %s.\n", path);
        int res = shm_unlink(path);
        if(res){
        printf("Error unlinking shared memory object at %s.\n", path);
        return -1;
        }
	printf("Successfully unlinked shared memory object at %s.\n", path);
	return 0;
}

