#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "strbuf.h"
#include "queue.h"

void queue_init(Queue* queue)
{
    queue->head = NULL;
    queue->rear = NULL;
    unsigned count = 0;
    queue->open = 1;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->read_ready, NULL);
	pthread_cond_init(&queue->write_ready, NULL);
}

void queue_insert(Queue* queue, char* string)
{
    pthread_mutex_lock(&queue->lock);
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
        pthread_mutex_unlock(&queue->lock);
        return;
    }

    queue->rear->next = node;
    queue->rear = node; 
    queue->count++;
    pthread_mutex_unlock(&queue->lock);
    return;

}

//must free return value
char* queue_dequeue(Queue* queue)
{
    pthread_mutex_lock(&queue->lock);
    //handles if empty
    if(queue->head==NULL)
    {
        //add condition wait
        pthread_mutex_unlock(&queue->lock);
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
    pthread_mutex_unlock(&queue->lock);
    return headWord;
}

void queue_destroy(Queue* queue)
{
    pthread_mutex_destroy(&queue->lock);
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

