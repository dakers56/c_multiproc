#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include<time.h>
#include <errno.h>
#include "queue.h"
#include <string.h>

#define SHMOBJ_PATH "/shm_sh_q_1"
#define SHMOBJ_DATA_PATH "/shm_sh_data"

#define Q_SEM "/q_sem"

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)
//implementing Queue using doubly-linked list
struct Q_NODE_ {
	QUEUE *q;
	Q_NODE *pred;
	Q_NODE *succ;
	char * val;
	};

int main(int argc, char * argv[]){
	printf("Creating new queue.\n");
	errno = 0;
	int shmfd = shm_open(SHMOBJ_PATH, O_CREAT| O_RDWR, 0666);
        if (shmfd == -1){
        handle_error("shm_open failed.");
        return -1;
        }
	printf("Created shared memory object with FD %d.\n", shmfd);
	ftruncate(shmfd, sizeof(QUEUE));
	QUEUE *q = init_sh(1000, shmfd);
	if(q == NULL){
		printf("Failed to intialize queue. Errno: %d\n", errno);
		return 1;
	}	
	printf("Successfully initialized queue.\n");
	printf("Initializing new node.\n");

	
	char *sh_data[]  = {"ABC", "DEF" ,"GHI", "JKL", "MNO", "PQR", "STU", "VWX", "YZ" };
	int sh_data_size = sizeof(sh_data);
	int len = sizeof(sh_data) / sizeof(sh_data[0]);

	printf("Printing elements of array before memcpy.\n");
	for (int i = 0; i < sizeof(sh_data) / sizeof(sh_data[0]); i++){
	printf("sh_data[%d]: %s\n", i, *(sh_data + i));
	}
	printf("---------------------\n");

	int shmfd_data = shm_open(SHMOBJ_DATA_PATH, O_CREAT| O_RDWR, 0666);
	ftruncate(shmfd_data, sh_data_size);
        if (shmfd == -1){
        handle_error("shm_open failed for shared data..");
        return -1;
        }
	
	int to_proc = sizeof(sh_data) / sizeof(sh_data[0]);

	char ** sh_data_map = (char **) mmap(NULL, sh_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_data, 0); 
	//memcpy(sh_data, sh_data_map, sizeof(sh_data)/ sizeof(char));
	
	printf("Printing elements of array after mmap.\n");
        for (int i = 0; i < 9; i++){
        printf("sh_data[%d]: %s\n", i, *(sh_data + i));
        }
        printf("---------------------\n");
        	
	printf("Printing elements of array from shared memory.\n");
        for (int i = 0; i < 9; i++){
	printf("Performing memcpy\n");	
	printf("sh_data_map + i: %u.\n", sh_data_map + i);
	printf("sh_data + i: %u.\n", sh_data + i);
	//memcpy(sh_data_map, sh_data + i, strlen(sh_data + i)); 
	strcpy(sh_data_map + i, sh_data + i); 
        printf("sh_data_map[%d]: %s\n", i, *(sh_data_map + i));
        }
        printf("---------------------\n");

	printf("Setting val to %s (addr: %d).\n", *sh_data, sh_data);
	printf("Enqueing sh_data[0]\n.");
	printf("q: %u\n", q);
	printf("sh_data: %u\n", sh_data);
	printf("*sh_data: %s\n", *sh_data);
	printf("*(sh_data + 1): %s\n", *(sh_data + 1));
	enqueue(q, sh_data);
	
//	printf("Forking process.\n");

	
	for(int i = 1; i < len; i++){
	printf("-------------------\n");
	printf("Preparing to enqueue node %d.\n", i);
	printf("Enqueueing node %d.\n", i);
	enqueue(q, *(sh_data + i));
	printf("-------------------\n");
	}

	sem_t *q_sem = sem_open(Q_SEM, O_CREAT, 0666, 1);	
	int pid = fork();
//	int pid = 1;
	if(pid == -1){
	handle_error("Fork failed. ");
	_exit(EXIT_FAILURE);
	}
        char *pr_nm;	
	if(pid == 0) pr_nm = "child";
	else pr_nm = "parent"; 

	while(!is_empty(q) || q->full){
		printf("-------------------\n");
		printf("Inside %s process\n", pr_nm);
		printf("Dequeuing element.\n");
		printf("%s process waiting on lock.\n", pr_nm);
		sem_wait(q_sem);
		printf("%s process obtained lock.\n", pr_nm);
		Q_NODE * qn = dequeue(q);
		//printf("Dequeued.\n");
		//printf("qn: %d.\n", qn);
		//printf("Dequeued value addr: %d.\n", qn->val);
		printf("Dequeued value: %s.\n", qn->val);
		printf("Num occupied: %d.\n", q->occupied);
		printf("%s process posting to semaphore.\n", pr_nm);
		sem_post(q_sem);
		printf("-------------------\n");
	}
	char * term_msg;
	if((q->occupied))
		term_msg = "Queue was full.\n";
	else 	term_msg = "All elements consumed from queue.\n";
	printf(term_msg);
/*	
	printf("%s process joining.\n");
	join();
	printf("All processes joined.\n");

*/	
	if(pid != 0)	{
	printf("Unmapping memory for queue.\n");
	int res = munmap(q, sizeof(QUEUE));	
	if(res){
	handle_error("Error unmapping memory. ");
	return -1;
	}

	printf("Unmapping memory for data.\n");
        res = munmap(sh_data_map, sizeof(sh_data_map));
        if(res){
        handle_error("Error unmapping memory. ");
        return -1;
        }	

	printf("Unlinking %s.\n", SHMOBJ_PATH);
	res = shm_unlink(SHMOBJ_PATH);
	 if(res){
        handle_error("Error unlinking shared memory object. ");
        return -1;
        }
	printf("Unlinking %s.\n", SHMOBJ_DATA_PATH);
        res = shm_unlink(SHMOBJ_DATA_PATH);
        if(res){
        handle_error("Error unlinking shared memory object. ");
        return -1;
        }
	
	res = sem_close(q_sem);
	printf("Closing  queue lock.\n");
	if (res){
		handle_error("Error closing semaphore");
		return -1;
	}
	
	res = sem_unlink(Q_SEM); 
	if (res) {
                handle_error("Error unlinking semaphore");	
		return -1;
	}
	printf("Parent process done cleaning up shared resources.\n");
	
}
	else{
	int status;
	(void) waitpid(pid, &status, 0);
	printf("Child process finished.\n");
	}
	return 0;
}
int size(QUEUE *q){
	return q->head - q->tail;
}

int enqueue(QUEUE *q, char * val){
	printf("address of val: %u\n", val);
	printf("*val: %c\n", *val);
	printf("val as string: %s\n", val);
	if(q->empty){
		//printf("Queue was empty\n");
		q->empty=0;
	}
	if(q->full){
	//printf("Queue was full, could not enqueue.\n");
	return -1;
	}
	(q->occupied)++;
	//printf("q->occupied: %d.\n", q->occupied);
	if(q->occupied == q->max_size) {
	//printf("Queue is now full.\n");
	q->full=1;
	}
	//printf("\n-----------------------------------------------------------\n");
	//printf("q->next_avail: %d.\n", q->next_avail);
	Q_NODE * node = q->next_avail;
	//printf("Setting successor of current tail to node given to enqueue.\n");
	//printf("node->pred: %d.\n", node->pred);
	//printf("q->tail: %d.\n", q->tail);
	node->q=q;
	node->pred=q->tail;
	//printf("node->pred: %d.\n", node->pred);
	node->succ=NULL;
	//printf("node->succ: %d.\n", node->succ);
	//printf("Setting successor of tail to noded being enqueued.\n");
	q->tail->succ=node;
	//printf("Making new node the tail of the queue.\n");
	//printf("val: %s.\n", val);
	q->tail=node;
	//printf("Setting val.\n");
	node->val=val;
	//printf("node->val: %s.\n", node->val); 
	//printf("-----------------------------------------------------------\n");
	set_next_avail(q, (Q_NODE *) NULL);	
	return 0;
}

void free_q(QUEUE *q){
	free(q->head);	
	free(q->tail);	
	free(q);
}

int init(QUEUE *q, int size){
	int default_size = 10000;
	if (size <= 0){
		//printf("Allocating default sized queue (%d). Size provided was %d.\n", default_size, size);
		size = default_size;
		//printf("Size:  %d.\n", size);
	}	
	//printf("\n-----------------------------------------------------------\n");
	//printf("(Before) q.max_size: %d\n",q->max_size);
	(q->max_size)=size;
	//printf("Queue size: %d\n", q->max_size);
	int * head = malloc(size * sizeof(int));
	if(!head){
		//printf("Could not allocate integer pointer for queue.head.\n");
		return 1;
	}
	//printf("head: %d\n", head);
	q->head=head;
	q->tail=q->head;
	//printf("q->head: %d\n", q->head);
	//printf("q->tail: %d\n", q->tail);
	q->empty=1;
	//printf("q->empty: %d\n", (int) q->empty);
	//printf("\n-----------------------------------------------------------\n");
	return 0;
}

Q_NODE *  dequeue(QUEUE *q){
	//printf("\n-----------------------------------------------------------\n");
	//printf("Checking if queue is empty.\n");
	if(q->empty){
		//printf("Queue underflow error: Attempting to dequeue from empty queue.\n");
		return 0;
	}
	//printf("Queue was not empty.\n");
	Q_NODE *  head_node = q->head;
	//printf("q->occupied: %d.\n", q->occupied);
	(q->occupied)--;
	if(q->occupied == 0){
	//printf("Queue is now empty.\n");
	q->empty = 1;
	}
	//printf("Location of head node: %d\n", head_node);
	if(q->head == q->tail){
		//printf("Setting q.empty to true.\n");
		q->empty=1;
	}	
	q->head=head_node->succ;
	set_next_avail(q, head_node);
	//printf("Returning value.\n");
	//clean(head_node);
	//printf("\n-----------------------------------------------------------\n");
	return head_node;
}

QUEUE * init_sh(int size, int shmfd_q){
	//printf("\n-----------------------------------------------------------\n");
	//printf("Initialzing shared queue.\n");
	if(size<=0){
        //printf("Must specify a max_size >= 0 for queue. Size was %d\n", size);
        return NULL;
        }
	//printf("Mapping memory.\n");
	//printf("Size of queue: %d.\n", sizeof(QUEUE));
	//printf("Size of queue node: %d.\n", sizeof(Q_NODE));
	QUEUE *q = (QUEUE *) mmap(NULL, sizeof(QUEUE), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_q, 0);
	if(q == -1){
        	handle_error("MMAP failed for queue.\n");
        	return (QUEUE *) NULL;
        }
	
	q->empty=1;
	q->full=0;
	Q_NODE * head = malloc(size * sizeof(Q_NODE));
	if(!head){
	//printf("Could not initialize  head for q node.\n");
	return NULL;
	}
	q->head=head;
	q->tail=head;
	q->next_avail=q->head;
	q->occupied=0;
	//printf("\n-----------------------------------------------------------\n");
	return q;
}

int share(QUEUE *q, const char* shm_nm){
	int shmfd = shm_open(shm_nm, O_RDWR, 0666); 
	if (shmfd == -1){ 
        handle_error("shm_open failed for queue contents");
        return -1;
        }

	int * contents = (int *)  mmap(NULL, sizeof(int) * q->max_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if(q == -1){
        handle_error("MMAP failed for shared queue contents.\n");
        return -1;
        }

	//printf("Assigning head of queue to mapped memory.\n");
	q->head=contents;
	q->tail=contents;
	q->empty=1;
	return 0;
}	

//pass *qn=NULL if enqueueing
void set_next_avail(QUEUE *q, Q_NODE *qn){
	//printf("-----------------------------------------------------------\n");
	if(q->full){
	//printf("WARNING: queue is full\n.");
	return;
	}
	if(q->empty){
		//printf("Queue was empty. Setting next available to head (%d).\n", q->head);
		q->next_avail=q->head;
		return;
	}	
	if (qn != NULL){
		//printf("Setting next available to qn (%d)\n", qn);
		q->next_avail=qn;
		return;
	}
	//printf("q->next_avail before increment: %d\n.", q->next_avail);
	//printf("sizeof(Q_NODE) %d\n.", sizeof(Q_NODE));
	q->next_avail += (1 * sizeof(Q_NODE));
	q->next_avail += 1;
	//printf("q->next_avail after increment: %d\n.", q->next_avail);
	//printf("-----------------------------------------------------------\n");
	return;
	}
//Used when dequeueing. Retains value, but clears queue info from node.	
void clean(Q_NODE *qn){
	//printf("\n-----------------------------------------------------------\n");
	//printf("Cleaning node.\n");
	qn->q=NULL;
	qn->pred=NULL;
	qn->succ=NULL;
	//printf("\n-----------------------------------------------------------\n");
}


int is_empty(QUEUE *q){
	return q->occupied == 0;
	}
