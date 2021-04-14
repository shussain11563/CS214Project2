#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>   
#include "strbuf.h"
#include "queue.h"

int main(int argc, char* argv[])
{
    Queue q;
    queue_init(&q);

    for(int i = 0; i < argc; i++)
    {
        queue_insert(&q, argv[i]);
    }

    for(int i = 0; i < argc; i++)
    {
        //printf("%s \n", test->node.data);
        //test= test->next;

        char* test = queue_dequeue(&q); 
        printf("%s \n", test);
        free(test);
        
    }
    queue_destroy(&q);
}