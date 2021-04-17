#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include<dirent.h>
#include <pthread.h>

#include "strbuf.h"
#include "queue.h"
#include "compare.h"
#include "wfd.h"



//directory thread
void* directThreadFunction(void *A)
{
    struct targs* args = A;
    printf("Currently on Thread %d\n", args->id);
    while(args->directoryQueue->head!=NULL)
    {
        //puts("Grabbing");
        char* filename =  queue_dequeue(args->directoryQueue);
        //puts("Grabbed");
        printf("Dequeued on Thread %d\n", args->id);
        DIR* dirp = opendir(filename);
        struct dirent* entry;

        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }
            //if(isSuffix(filename,suffix)==1 && isFile(filename)==1)
            char* comboString = generateFilePath(filename, entry->d_name);
            char* temp[strlen(comboString)+1];
            strcpy(temp, comboString);

            if(isFile(comboString)==1 && isSuffix(comboString,args->suffix))
            {
                //puts("File:");
                //puts(comboString);
                queue_insert(args->fileQueue,temp);
                //printf("Inserted File Thread %d\n", args->id);
            }
            else if(isDir(comboString)==1)
            {   
                // puts("Directory:");
                //puts(comboString);
                //printf("Inserted on Thread %d\n", args->id);
                queue_insert(args->directoryQueue,temp);
            }
            //else
            //{   
                //puts(comboString); 
                //free(comboString);
            //}
            free(comboString);
            //printf("Faied to read Directory. The entry was %s  \n", entry->d_name);

        }
        closedir(dirp);
        free(filename);
        //open function
    }

}
//use this as a basis for our file thread
void* fileThreadFunction(void *A)
{
    struct targs* args = A;
    
    while(args->fileQueue->head!=NULL)
    {
        
        char* filename = queue_dequeue(args->fileQueue);

        puts(filename);
        //add to wfd repo



        free(filename);



    }
    
}

int main(int argc, char* argv[])
{
    int directoryThreads = 1;
    int fileThreads = 1;
    int analysisThreads = 1;
    int threads;
    //char* suffix = ".txt";
    char* suffix = NULL;
    Queue directoryQueue;
    Queue fileQueue;

    queue_init(&directoryQueue);
    queue_init(&fileQueue);


    for(int i = 1; i < argc; i++)
    {
        if(isDir(argv[i])==1)
        {
            queue_insert(&directoryQueue, argv[i]);
            //test(&directoryQueue, &fileQueue, ".txt");
        }
        else if(isFile(argv[i])==1)
        {
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
            //puts(suffix);
            //free(suffix); // --->Add to end
        }
        else
        {
            //illegal argument
        }
    }

    if(suffix==NULL)
    {
        char* temp = ".txt";
        suffix = malloc(sizeof(char)*(strlen(temp)+1));
        strcpy(suffix, temp);
    } 

    threads = directoryThreads+fileThreads+analysisThreads;
    threads = directoryThreads+fileThreads; //remove

    pthread_t* tids = malloc(sizeof(pthread_t) * threads);
    struct targs* args = malloc(sizeof(struct targs) * threads);

    //args.directoryQueue = &directoryQueue;
    //args.fileQueue = &fileQueue;
    //args.suffix = suffix;


    int i = 0;
    for(; i <directoryThreads; i++)
    {
        args[i].directoryQueue = &directoryQueue;
        args[i].fileQueue = &fileQueue;
        args[i].suffix = suffix;
        args[i].id =  i;
        pthread_create(&tids[i], NULL, directThreadFunction, &args[i]);
    }

    //for(; i <threads; i++)
    //{
        //pthread_create(&tids[i])
    //}
    int j=0;
    for (; j < directoryThreads; ++j) 
    {
		pthread_join(tids[j], NULL);
	}
    
    //experiemnt
    sleep(5);
    for(; i <threads; i++)
    {
        args[i].directoryQueue = &directoryQueue;
        args[i].fileQueue = &fileQueue;
        args[i].suffix = suffix;
        args[i].id =  i;
        pthread_create(&tids[i], NULL, fileThreadFunction,&args[i]);
    }
    
    for (; j < threads; ++j) 
    {
		pthread_join(tids[j], NULL);
	}


    


    free(args); //remove


    puts(suffix);
    free(tids);
    //free(args);
    free(suffix);
    queue_destroy(&directoryQueue);
    queue_destroy(&fileQueue);
    return EXIT_SUCCESS;


    //close queue function

}