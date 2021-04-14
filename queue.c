#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"
#include "queue.h"

void queue_init(Queue* queue)
{
    queue->head = NULL;
    queue->rear = NULL;
    unsigned count = 0;
}

void queue_insert(Queue* queue, char* string)
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
char* queue_dequeue(Queue* queue)
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

void queue_destroy(Queue* queue)
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

