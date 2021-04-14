#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>   
#include "strbuf.h"
#include "queue.h"

int main(int argc, char* argv[])
{
    int directoryThreads = 1;
    int fileThreads = 1;
    int analysisThreads = 1;
    char* suffix = ".txt";
    Queue directoryQueue;
    Queue fileQueue;

    queue_init(&directoryQueue);
    queue_init(&fileQueue);


    for(int i = 1; i < argc; i++)
    {
        if(isDir(argv[i])==1)
        {
            //add to directory queue
            queue_insert(&directoryQueue, argv[i]);
        }
        else if(isFile(argv[i])==1)
        {
            //suffix stuff
            queue_insert(&fileQueue, argv[i]);
        }
        else if(strncmp(argv[i], "-d", 2)==0)
        {
            printf("%d\n", threadCount(argv[i], "-d")); 
            directoryThreads = threadCount(argv[i], "-d");

            //add error checsks for -1,  0
        }
        else if(strncmp(argv[i], "-f", 2)==0)
        {
            fileThreads = threadCount(argv[i], "-f");
        }
        else if(strncmp(argv[i], "-a", 2)==0)
        {
            analysisThreads = threadCount(argv[i], "-a");
        }
        else if(strncmp(argv[i], "-s", 2)==0)
        {
            //malloc
            suffix = stringParser(argv[i], "-s");
            puts(suffix);
            free(suffix); // --->Add to end
        }
        else
        {
            //illegal argument
        }
    }




    queue_destroy(&directoryQueue);
    queue_destroy(&fileQueue);
    //close queue function


    /*
    ///////////////////////////
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
    */
}