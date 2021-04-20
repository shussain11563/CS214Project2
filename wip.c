typedef struct {
	int data[QSIZE];
	unsigned count;
	unsigned head;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} queue_t;