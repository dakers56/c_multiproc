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
#define SHMOBJ_PATH "/shm_sh_q_1"
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
	printf("Initializing queue.\n");
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
	printf("Unmapping memory.\n");
	int res = munmap(q, sizeof(QUEUE));	
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
	/*
	void (*foo) (QUEUE*, int);
	foo = enqueue;
	int to_enqueue[] = {1,2,3,4,5};
	for (int i = 0; i < 5; i++){
		printf("Pushing %d onto queue.\n", to_enqueue[i]);
		foo(q, to_enqueue[i]);
	}
	
	for (int i = 0; i < 5; i++){
		printf("Value dequeued from queue: %d\n", dequeue(q));
	}
	printf("Successfully enqueueed and dequeueed values.\n");
	*/
	Q_NODE * n = new_node(q);
	return 0;
}
int size(QUEUE *q){
	return q->head - q->tail;
}

int enqueue(QUEUE *q, Q_NODE *node){
	if(q->empty){
		printf("Queue was empty\n");
		q->empty=0;
	}
	if(q->full){
	printf("Queue was full, could not enqueue.\n");
	return -1;
	}
	printf("Setting successor of current tail to node given to enqueue.\n");
	node->pred=q->tail;
	q->tail->succ=node;
	q->tail=node;
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
		printf("Allocating default sized queue (%d). Size provided was %d.\n", default_size, size);
		size = default_size;
		printf("Size:  %d.\n", size);
	}	
	printf("(Before) q.max_size: %d\n",q->max_size);
	(q->max_size)=size;
	printf("Queue size: %d\n", q->max_size);
	int * head = malloc(size * sizeof(int));
	if(!head){
		printf("Could not allocate integer pointer for queue.head.\n");
		return 1;
	}
	printf("head: %d\n", head);
	q->head=head;
	q->tail=q->head;
	printf("q->head: %d\n", q->head);
	printf("q->tail: %d\n", q->tail);
	q->empty=1;
	printf("q->empty: %d\n", (int) q->empty);
	return 0;
}

Q_NODE *  dequeue(QUEUE *q){
	printf("Checking if queue is empty.\n");
	if(q->empty){
		printf("Queue underflow error: Attempting to dequeue from empty queue.\n");
		return 0;
	}
	printf("Queue was not empty.\n");
	Q_NODE *  head_node = q->head;
	printf("Location of head node: %d\n", head_node);
	q->head=head_node->succ;
	if(q->head == NULL){
		printf("Setting q.empty to true.\n");
		q->empty=1;
	}
	set_next_avail(q, head_node);
	printf("Returning value.\n");
	return head_node;
}

QUEUE * init_sh(int size, int shmfd_q){
	printf("Initialzing shared queue.\n");
	if(size<=0){
        printf("Must specify a max_size >= 0 for queue. Size was %d\n", size);
        return NULL;
        }
	printf("Mapping memory.\n");
	printf("Size of queue: %d.\n", sizeof(QUEUE));
	QUEUE *q = (QUEUE *) mmap(NULL, sizeof(QUEUE), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_q, 0);
	if(q == -1){
        	handle_error("MMAP failed for queue.\n");
        	return (QUEUE *) NULL;
        }
	q->contents = malloc(sizeof(char) * size); 
        if(!(q->contents)){
	printf("Could not allocate memory for contents of queue.\n");
	return (Q_NODE *) NULL;
	}
	q->empty=1;
	q->full=0;
	q->head=(Q_NODE *) NULL;
	q->tail= (Q_NODE *) NULL;
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

	printf("Assigning head of queue to mapped memory.\n");
	q->head=contents;
	q->tail=contents;
	q->empty=1;
	return 0;
}	

Q_NODE  * new_node(QUEUE * q){
	 Q_NODE * qn = malloc(sizeof(Q_NODE));
	 if(!qn){
		printf("Error allocating queue node\n");
		return (Q_NODE *) NULL;
	}
	qn->q=q;
	qn->pred=(Q_NODE *) NULL;
	qn->succ=(Q_NODE *) NULL;
	qn->val=(char *) NULL;
	return qn;
}

//pass *qn=NULL if enqueueing
void set_next_avail(QUEUE *q, Q_NODE *qn){
	if(q->full){
	printf("WARNING: queue is full\n.");
	return;
	}
	if(q->empty){
		printf("Queue was empty. Setting next available to beginning of contents (%d).\n", q->contents);
		q->next_avail=q->contents;
		return;
	}	
	if (qn != NULL){
		printf("Setting next available to qn (%d)\n", qn);
		q->next_avail=qn;
		return;
	}
	printf("q->next_avail before increment: %d\n.", q->next_avail);
	q->next_avail += 1 * sizeof(QUEUE);
	printf("q->next_avail after increment: %d\n.", q->next_avail);
	return;
	}
	
