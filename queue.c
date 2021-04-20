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
    queue->count = 0;
    queue->open = 1;
    queue->activeThread = 0;
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

    if(queue->count==0)
    {
        queue->head = node;
        queue->rear = node;
        queue->count++;
        //printf("hello insert %d\n",queue->count);
        pthread_mutex_unlock(&queue->lock);
        pthread_cond_signal(&queue->read_ready);
        
        return;
    }

    queue->rear->next = node;
    queue->rear = node; 
    queue->count++;
    //printf("hello insert %d\n",queue->count);
    pthread_cond_broadcast(&queue->read_ready);
    pthread_mutex_unlock(&queue->lock);
    return;

}

//must free return value
char* queue_dequeue_dir(Queue* dirQueue, Queue* fileQueue)
{
    pthread_mutex_lock(&dirQueue->lock);
    //handles if empty
    //printf("Queue %d\n",dirQueue->count);
    //printf("No active threads left %d \n", dirQueue->activeThread);

    //printf("No size left %d \n", dirQueue->count);
    if(dirQueue->count==0) // if queue->head is null and queue is opne, wait
    {
        dirQueue->activeThread--;
        if(dirQueue->activeThread==0)
        {
            //pthread_cond_broadcast(&dirQueue->read_ready);
            pthread_mutex_unlock(&dirQueue->lock);
            pthread_cond_broadcast(&dirQueue->read_ready); 
            pthread_cond_broadcast(&fileQueue->read_ready); 
            return NULL;
        }
        while(dirQueue->count==0 && dirQueue->activeThread!=0)
        {
            pthread_cond_wait(&dirQueue->read_ready, &dirQueue->lock);
        }
        if(dirQueue->activeThread==0)
        {
            pthread_mutex_unlock(&dirQueue->lock);
            return NULL;
        }
        dirQueue->activeThread++;

    }

    //grabs the first head
    //puts("This message should only occur once");
    strbuf_t_node* retHead = dirQueue->head;

    strbuf_t temp = retHead->node;
    //retHead->rear = NULL; //remove this?
    dirQueue->head = dirQueue->head->next;
    //printf("No size left %d \n", dirQueue->count);
    dirQueue->count--;
    //printf("No size left %d \n", dirQueue->count);

    if(dirQueue->head==NULL)
    {
        dirQueue->rear=NULL;
    }
    char* headWord = retHead->node.data;
    free(retHead);
    pthread_mutex_unlock(&dirQueue->lock);
    return headWord;
}


char* queue_dequeue_file(Queue* fileQueue, Queue* dirQueue)
{
    pthread_mutex_lock(&fileQueue->lock);
    //handles if empty
    //puts("We are inside dequeue");
    if(fileQueue->count==0)
    {
       // puts("No files left");
        //printf("No active threads left %d \n", dirQueue->activeThread);
        if(dirQueue->activeThread == 0)
        {
            pthread_mutex_unlock(&fileQueue->lock);
            pthread_cond_broadcast(&fileQueue->read_ready);
            return NULL;
        }
        while(fileQueue->count==0 && dirQueue->activeThread!=0)
        {
            pthread_cond_wait(&fileQueue->read_ready, &fileQueue->lock);
        }
        if(fileQueue->count==0)
        {
            pthread_mutex_unlock(&fileQueue->lock);
            return NULL;
        }

    }
    
    strbuf_t_node* retHead = fileQueue->head;
    strbuf_t temp = retHead->node;
    //retHead->rear = NULL; //remove this?
    fileQueue->head = fileQueue->head->next;
    fileQueue->count--;

    if(fileQueue->head==NULL)
    {
        fileQueue->rear=NULL;
    }
    char* headWord = retHead->node.data;
    free(retHead);
    pthread_mutex_unlock(&fileQueue->lock);
    return headWord;
}


void queue_close(Queue* queue)
{
	pthread_mutex_lock(&queue->lock);
	queue->open = 0;
	pthread_cond_broadcast(&queue->read_ready);
	pthread_cond_broadcast(&queue->write_ready);
	pthread_mutex_unlock(&queue->lock);	

	return;
}

void queue_destroy(Queue* queue)
{
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->read_ready);
    pthread_cond_destroy(&queue->write_ready);
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

