typedef struct strbuf_t_node
{
    strbuf_t node;
    struct strbuf_t* next;
} strbuf_t_node;

typedef struct Queue
{
    strbuf_t_node* head;
    strbuf_t_node* rear;
    int count;
    int activeThread;
    int open;
    pthread_mutex_t lock;
    pthread_cond_t read_ready;
	pthread_cond_t write_ready;
    
} Queue;

void queue_init(Queue* );
void queue_insert(Queue* , char* );
char* queue_dequeue_dir(Queue* dirQueue, Queue* fileQueue);
char* queue_dequeue_file(Queue* fileQueue, Queue* dirQueue);
void queue_destroy(Queue* );
void queue_close(Queue* );