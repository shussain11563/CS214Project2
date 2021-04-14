typedef struct strbuf_t_node
{
    strbuf_t node;
    struct strbuf_t* next;
} strbuf_t_node;

typedef struct Queue
{
    strbuf_t_node* head;
    strbuf_t_node* rear;
    unsigned count;
    
} Queue;

void queue_init(Queue* );
void queue_insert(Queue* , char* );
char* queue_dequeue(Queue* );
void queue_destroy(Queue* );