#ifndef QUEUE_H_
#define QUEUE_H_

struct queue {
	char empty:1;
	int *head;
	int *tail;
	int max_size; 
};

typedef struct queue QUEUE;
struct Q_NODE_;
typedef struct Q_NODE_ Q_NODE;
/*
struct shared_queue{
	QUEUE * q;
	char shm [];
	char sem [];	
	int * contents;
}
typedef struct shared_queue SHARED_QUEUE;*/
Q_NODE * new_node();
int size (QUEUE *q);
void enqueue(QUEUE *q, int i);
int dequeue(QUEUE *q);
void free_q(QUEUE *q);
int init(QUEUE *q, int size);
QUEUE * init_sh(int size, int shmfd);
int share(QUEUE *q, const char* shm_nm);

#endif
