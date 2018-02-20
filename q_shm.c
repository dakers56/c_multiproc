#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/mman.h>
#include <errno.h>
#include<semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#include "digits.h"
#include "q_shm.h"
#include "shm_helper.h"

char sh_name_base[] = "/Q_SHARE_OBJ_";
int sh_id = 0;

//char sem_name_base[] =  "/Q_SHARE_OBJ_SEM_";
char sem_name_base[] =  "/Q2_SEM_";
int sem_id = 0;

char *name();
char *sh_name();
char *sem_name();

int main(int argc, char ** argv){
	if(argc == 2){
		sem_id = (int) argv[1];
		sh_id =  (int) argv[1];
	}
	printf("sem_id: %d; sh_id: %d.\n");
	printf("Creating SHARE object.\n");
	SHARE *sh = new_share(24);
	if(sh == NULL) {
		printf("Error creating share object.\n");
		return -1;
	}
	/*	
	printf("Successfully created SHARE object.\n");
	printf("\n--------------------------\n");
	printf("Attributes of SHARE object.\n");
	printf("sh: %d.\n", sh);
	printf("shm_nm: %d.\n", sh->shm_nm);
	printf("fd: %d.\n", sh->fd);
	printf("max_size: %d.\n", sh->max_size);
	printf("sh_id: %d.\n", sh->sh_id);
	printf("sem_id: %d.\n", sh->sem_id);
	//printf("*sem_t: %u.\n", sh->sem_t_);
	printf("name:  %s.\n", sh->name);
	printf("sem_nm:  %s.\n", sh->sem_nm);
	printf("base:  %u.\n", sh->base);
	printf("next:  %u.\n\n", sh->next);
	printf("\n--------------------------\n");
	*/
	sem_t *sh_sem = get_sem(sh);
	printf("Waiting on semaphore.\n");
	pid_t pid;
	pid = fork();
	char *pr_name;
	if(pid == -1) {
		printf("Fork failed.\n");
		return -1;
	}
	if (pid == 0){
		pr_name = malloc(strlen("child"));
		strcpy(pr_name, "child");
	}
	else{
	printf("PID: %d.\n", pid);
	pr_name = malloc(strlen("parent"));
	strcpy(pr_name, "parent");
	}
	printf("Inside %s process.\n", pr_name);
	sem_wait(sh_sem);
	printf("Obtained lock.\n");
	printf("Doing something...\n");
	sem_post(sh_sem);
	printf("%s:posting to semaphore.\n", pr_name);
	if(strcmp(pr_name,"parent") == 0){ 
		sem_close(sh_sem);	
		printf("Closed semaphore.\n");
		char *sem_nm = malloc(strlen(sh->sem_nm));
		strcpy(sem_nm, sh->sem_nm);
		printf("Unliking semaphore %s.\n", sem_nm);
		sem_unlink(sem_nm);
		printf("Unliked semaphore %s.\n", sem_nm);
		free(sem_nm);
		printf("Destroying shared memory object.\n");
		destroy(sh, NULL);
	}
	printf("%s process completed.\n", pr_name);
	free(pr_name);
	printf("Done freeing memory.\n");
	return 0;
}

SHARE * new_share(int size){
	printf("ENOENT: %d\n", ENOENT);
	char *name_ = sh_name();
	int res = helper_unlink(name_);
	if(res == -1) return -1;
	int fd = helper_open_default(name_, sizeof(SHARE));
	if (fd == -1) return NULL;

	SHARE *new_sh =(SHARE *) mmap(NULL, sizeof(SHARE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 	
	if(new_sh == MAP_FAILED){
		perror("Error mapping SHARE object into shared memory.\n");
		return NULL;	
	}
	new_sh->fd=fd;
	new_sh->name=name_;
	new_sh->sh_id=sh_id;
	new_sh->max_size=size;
	
	char *sem_nm_ = sem_name();
	printf("sem name is %s.\n", sem_nm_);
	new_sh->sem_nm=sem_nm_;
	printf("Set sem name to %s.\n", new_sh->sem_nm);
	//TODO: Share name?
	//char *shmfd_nm =(char *) malloc(sizeof(char) * strlen(name_));
	

	/*
	int sz = sizeof(char) * 200;
	char *shmfd_nm_ = "/shm_name";
	int shm_nm_fd = helper_open_default(shmfd_nm_, sz);
	if (shm_nm_fd == -1) return NULL;
        char *shm_nm_ = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, shm_nm_fd, 0); 
	strcpy(shm_nm_, name_);
	strcat(shm_nm_, "_name");
	new_sh->shm_nm=shm_nm_; 
	*/	
	
	/*
	new_sh->shm_nm=shm_nm_;
	res = helper_unlink(shm_nm_);
        if(res == -1) return -1;
	*/
	
	int sz = 100;
	char *shmfd_nm = malloc(sizeof(char) * sz);
	printf("Start - shmfd_nm: %s\n", shmfd_nm);
	strcpy(shmfd_nm, name_);
	printf("Middle - shmfd_nm: %s\n", shmfd_nm);
        strcat(shmfd_nm, "_mem");
	printf("End - shmfd_nm: %s\n", shmfd_nm);
	
	
	int shm_contents_fd =  helper_open_default(shmfd_nm,sz);
	if(shm_contents_fd == -1){
                perror("Error opening file for shared memory contents' name.\n");
                return NULL;
        }
	char *shm_contents_nm = (char *) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, shm_contents_fd, 0);
	if (shm_contents_nm == -1){
		perror("Error mapping memory for shared memory contents' name.\n");
		return NULL;
	}
	strcpy(shm_contents_nm, shmfd_nm);	
	printf("Copied shared memory contents' name to shared memory.\n");
	printf("shm_contents_nm: %s.\n", shm_contents_nm);
	new_sh->shm_nm=shm_contents_nm;
	
	int shmfd = helper_open_default(shmfd_nm, size);
	if(shmfd == -1){
		perror("Error opening file for shared memory for SHARE object.\n");
		return NULL;	
	}
	char *shm_ptr= (char *) mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if(shm_ptr == MAP_FAILED){
		perror("Error mapping memory for contents of shared memory for SHARE object.\n");
		return NULL;
	}
	new_sh->base=shm_ptr;
	new_sh->next=shm_ptr;	
	return new_sh;
	}

int destroy(SHARE *sh, sem_t *sem_t_){
	printf("Destroying SHARE object \'%s\'.\n", sh->name);
	printf("sem_t_ \'%d\'.\n", sem_t_);
	printf("sh->shm_nm: %s.\n" , sh->shm_nm);

	//first, destroy semaphore
	int fail;
	if(sem_t_ != NULL){
	printf("Waiting on lock.\n");
	printf("Before sem_wait: sh->sem_t_ = %d\n", sem_t_);
	fail = sem_wait(sem_t_);
	if(fail){
		perror("Error during call to sem_wait");
		return -1;
	}
	printf("After sem_wait: sh->sem_t_ = %d\n", sem_t_);
	printf("After sem_wait: fail = %d\n", fail);
	if(fail){
                perror("sem_wait failed.\n");
                return -1;
        }
	printf("Obtained lock.\n");	
	printf("Posting to semaphore.\n");	
	printf("Before post: sh->sem_t_ = %d\n", sem_t_);
	sem_post(sem_t_);
	printf("Posted to semaphore.");
	fail = sem_close(sem_t_);
	if(fail){
                perror("sem_close failed");
                return -1;
        }
	printf("Closed semaphore.");
	fail = sem_unlink(sem_t_);
	if(fail){
		perror("Could not unlink semaphore\n");
		return -1;
	}	
	sh->sem_id=0;
}
	else {
		printf("No semaphore provided.\n");
	}
	printf("sh->base: %d.\n", sh->base);
	printf("sh->max_size: %d.\n", sh->max_size);
	fail = munmap(sh->base, sh->max_size);
	if (fail) {
		perror("Error unmapping contents of shared memory.\n");
		return -1;
	}	
//	sh->max_size=0;
	printf("Successfully unmapped contents of queue from shared memory.\n");
	
	printf("Another line.\n");
	//printf("sh->shm_nm: %s.\n" , sh->shm_nm);
	fail = helper_unlink(sh->shm_nm);
	if(fail){
		perror("Error unlinking shm_nm");
		return -1;
		}			
	printf("Done unlinking shm_nm.\n");


	printf("sh: %d.\n", sh);
	fail = munmap(sh, sizeof(SHARE));
	if(fail){
		perror("Error unmapping SHARE object");
		return -1;
	}
	printf("Successfully unmapped SHARE object from memory.\n");
		//sh->sh_id=0;

	}

char *name(char *name_base, int id){
	char *name_cp = malloc(sizeof(sh_name_base));
        strcpy(name_cp, name_base);
        char *digs=ch_digits(id);
        strcat(name_cp, digs);
        free(digs);
        return name_cp;
}

char *sh_name(){
	return name(sh_name_base, ++sh_id);
}

char *sem_name(){
	return name(sem_name_base, ++sem_id);
	}

sem_t *get_sem(SHARE *sh){
	/*
        int fail = sem_unlink(sh->sem_nm);
        if(fail){
		printf("Semaphore already existed.\n");
        }
	else printf("Created semaphore.\n");
	*/
	printf("Created semaphore.\n");
	sem_t *s = sem_open(sh->sem_nm, O_CREAT, 0666, 1);
	if(s == SEM_FAILED){
		perror("Error creating semaphore for SHARE object.\n");
		return NULL;
	}
	printf("Opened semaphore successfully.\n");
	return s;
}

int close_sem(char *sem_nm){
        int fail = sem_unlink(sem_nm);
        if(fail){
                perror("Failed to open semaphore ");
        }
       	else printf("Successfully created semaphore ");
	return fail;
}
