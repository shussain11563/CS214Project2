#include <stdlib.h>
#include <stdio.h>
#include "compare.h"
#include <string.h>
#include "strbuf.h"
#include<ctype.h>
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


void init(Queue* queue)
{
    queue->head = NULL;
    queue->rear = NULL;

    unsigned count = 0;
}

void insert(Queue* queue, char* string)
{
    
    strbuf_t word;
    sb_init(&word, 5);
    sb_concat(&word, string);
    strbuf_t_node* node = malloc(sizeof(strbuf_t_node));
    node->node = word;
    node->next= NULL;

    if(queue->rear==NULL)
    {
        queue->head = node;
        queue->rear = node;
        return;
    }

    queue->rear->next = node;
    queue->rear = node; 
    queue->count++;

}

//must free return value
char* dequeue(Queue* queue)
{
    //handles if empty
    if(queue->head==NULL)
    {
        return; //EMPTY //change this
    }

    //grabs the first head
    strbuf_t_node* retHead = queue->head;

    strbuf_t temp = retHead->node;
    //retHead->rear = NULL; //remove this?
    queue->head = queue->head->next;
    queue->count--;

    if(queue->head==NULL)
    {
        queue->rear=NULL;
    }


    
    char* headWord = retHead->node.data;
    free(retHead);
    return headWord;
}

void destroy(Queue* queue)
{
    strbuf_t_node* prev = NULL;
    strbuf_t_node* ptr = queue->head;
    while(ptr!=NULL)
    {
        prev = ptr;
        ptr = ptr->next;
        free(prev->node.data);
        free(prev);
    }

}





/*
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
*/

//put strbuf structure 


int main(int argc, char* argv[])
{
    Queue q;
    init(&q);

    for(int i = 0; i < argc; i++)
    {
        //insert(&q, argv[i]);
    }

    for(int i = 0; i < argc; i++)
    {
        //printf("%s \n", test->node.data);
        //test= test->next;

        char* test = dequeue(&q); 
        printf("%s \n", test);
        free(test);
        
    }
    destroy(&q);
}
