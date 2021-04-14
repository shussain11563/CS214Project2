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
/*
typedef struct Queue
{
    char** array;
    int count;
    int size;


} Queue;
*/

void init(Queue* queue, int count)
{
    queue->size = count;
    queue->count = 0;
    queue->array = malloc(sizeof(char*)  * queue->size);
    for(int i = 0; i < queue->size; i++)
    {
        queue->array[i] = malloc(sizeof(char)*10);
    }


}

void insert(Queue* queue, char* word)
{
    strcpy((queue->array)[queue->count], word);
    queue->count++;
}

void destroy(Queue* queue)
{
    for(int i = 0; i < queue->size; i++)
    {
        free(queue->array[i]);
    }
    free(queue->array);

}


//put strbuf structure 


int main(int argc, char* argv[])
{
    Queue q;
    init(&q, argc);

    for(int i = 0; i < argc; i++)
    {
        insert(&q, argv[i]);
    }
    for(int i = 0; i < q.size; i++)
    {
        printf("----> %s\n", q.array[i]);
    }
    destroy(&q);
}
