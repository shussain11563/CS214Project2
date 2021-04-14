#include <stdlib.h>
#include <stdio.h>
#include "compare.h"
#include <string.h>
#include "strbuf.h"
#include<ctype.h>


#ifndef DEBUG
#define DEBUG 0
#endif

//directory queue

typedef struct Queue
{
    unsigned size;
    strbuf_t* array;
    unsigned count;
    unsigned head;

} Queue;


void init(Queue* queue)
{
    queue->size = 3;  //test with realloc
    queue->count = 0;
    queue->head = 0;
    queue->array = malloc(sizeof(strbuf_t) * queue->size);

}



void insert(Queue* queue, char* string)
{
    if(queue->count+1==queue->size)
    {
        size_t size = queue->size+1;
        strbuf_t* temp = realloc(queue->array, sizeof(strbuf_t)*size);  
        queue->array = temp;
        queue->size = size;
    }
    strbuf_t word;
    sb_init(&word, 5);
    sb_concat(&word, string);
    queue->array[queue->count] = word;
    queue->count++;
}

void dequeue(Queue* queue, char* string)
{

}

void destroy(Queue* queue)
{
    for(int i = 0; i < queue->count; i++)
    {
        free(queue->array[i].data);
    }
    free(queue->array);
}


//put strbuf structure 


int main(int argc, char* argv[])
{
    Queue q;
    init(&q);

    for(int i = 0; i < argc; i++)
    {
        insert(&q, argv[i]);
    }
    for(int i = 0; i < q.count; i++)
    {
        printf("%d----> %s\n", q.count, q.array[i].data);
    }
    destroy(&q);
}
