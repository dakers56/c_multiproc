#ifndef QUEUE_H_
#define QUEUE_H_

struct Q_NODE_;
typedef struct Q_NODE_ Q_NODE;

struct queue {
	char empty:1;
	char full:1;
	Q_NODE *next_avail;
	Q_NODE *head;
	Q_NODE *tail;
	int max_size; 
	Q_NODE *contents;
	int occupied;
};

typedef struct queue QUEUE;
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
int enqueue(QUEUE *q, Q_NODE *node);
Q_NODE * dequeue(QUEUE *q);
void free_q(QUEUE *q);
int init(QUEUE *q, int size);
QUEUE * init_sh(int size, int shmfd);
int share(QUEUE *q, const char* shm_nm);
void set_next_avail(QUEUE *q, Q_NODE *qn);

#endif
