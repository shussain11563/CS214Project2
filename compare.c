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
#include "wfd.h"
#include "compare.h"

void* analysis(void *A)
{
    
}

//directory thread
void* directThreadFunction(void *A)
{
    struct targs* args = A;
    
    pthread_mutex_lock(&args->directoryQueue->lock);
    args->directoryQueue->activeThread++;
    pthread_mutex_unlock(&args->directoryQueue->lock);
    //while(args->directoryQueue->count!=0 && args->directoryQueue->activeThread!=0)

    //printf("Currently on Thread %d\n", args->id);
    char* filename = queue_dequeue_dir(args->directoryQueue, args->fileQueue);
    while(filename)
    {
        //char* filename =  queue_dequeue_dir(args->directoryQueue, args->fileQueue);
        if(filename==NULL)
        {
            return;
        }
        //printf("Dequeued on Thread %d\n", args->id);
        
        DIR* dirp = opendir(filename);
        //printf("Dequeued on Thread %d\n", args->id);
        struct dirent* entry;

        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }
            char* comboString = generateFilePath(filename, entry->d_name);
            char* temp[strlen(comboString)+1];
            strcpy(temp, comboString);
            

            if(isFile(comboString)==1 && isSuffix(comboString,args->suffix))
            {
                //printf("Inserted File Thread  ?? %d\n", args->id);
                queue_insert(args->fileQueue,temp);
                //pthread_cond_broadcast(&(args->fileQueue->read_ready));
                //printf("Inserted File Thread %d\n", args->id);
            }
            else if(isDir(comboString)==1)
            {  
                //printf("Inserted Directory Thread %d\n", args->id);
                queue_insert(args->directoryQueue,temp);
            }
            free(comboString);
        }
        
        closedir(dirp);
        free(filename);
        filename =  queue_dequeue_dir(args->directoryQueue, args->fileQueue);
        //open function
    }
    return;

}
//use this as a basis for our file thread
//use this as a basis for our file thread
void* fileThreadFunction(void *A)
{
    //sleep(1);
    struct targs* args = A;
 
    //puts("Entering File Thread");
    char* filename =  queue_dequeue_file(args->fileQueue, args->directoryQueue);
    //char* filename;
    while(filename)
    {
        
        //puts("Waiting to Dequeue in File Thread");
        
        /*
        if(filename==NULL)
        {
            return;
            puts("Ending File Thread because null");
        }*/
        //struct node* test=NULL;
        //puts("Successful in Dequeue in File Thread");
        //char* filename = queue_dequeue(args->fileQueue);
        


        //puts(filename);
        //free(filename);
        

        char temp[strlen(filename)+1];
        strcpy(temp, filename);
        puts("Hello");

        int file = open(temp, O_RDONLY);
        struct node* test = wfd(file);

        close(file);

        wfd_repo_insert(args->repo, test);


        


        
        puts("Goodbye");
        
        //args->repo = wfd_repo_insert(args->repo, test);
        //add to wfd repository
        //puts(filename);
        //add to wfd repo
        //free(filename);
        //puts("Deqeuing");
        puts(filename);
        free(filename);
        filename =  queue_dequeue_file(args->fileQueue, args->directoryQueue);
    }
    //puts("Ending File Thread");
    return;
}

int main(int argc, char* argv[])
{
    int directoryThreads = 1;
    int fileThreads = 1;
    int analysisThreads = 1;
    int collection_thread_count;
    //char* suffix = ".txt";
    char* suffix = NULL;
    Queue directoryQueue;
    Queue fileQueue;
    wfdRepo repo;
   

    queue_init(&directoryQueue);
    queue_init(&fileQueue);
    wfd_repo_init(&repo);



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
    collection_thread_count = directoryThreads+fileThreads; //remove
    pthread_t* tids = malloc(sizeof(pthread_t) * collection_thread_count+analysisThreads);
    struct targs* args = malloc(sizeof(struct targs) * collection_thread_count+analysisThreads);


    /*
    strbuf_t_node* prev = NULL;
    strbuf_t_node* ptr = directoryQueue.head;
    while(ptr!=NULL)
    {
        prev = ptr;
        ptr = ptr->next;
        char* f =  queue_dequeue_file(&directoryQueue, &fileQueue);
        puts(f);
        free(f);
    }
    

    */


    int i = 0;
    for(int i=0; i <directoryThreads; i++)
    {
        args[i].directoryQueue = &directoryQueue;
        args[i].fileQueue = &fileQueue;
        args[i].suffix = suffix;
        args[i].id =  i;
        args[i].repo = &repo;
        pthread_create(&tids[i], NULL, directThreadFunction, &args[i]);
    }

    //puts("Finished Directory Threads");
    for(; i <collection_thread_count; i++)
    {
        args[i].directoryQueue = &directoryQueue;
        args[i].fileQueue = &fileQueue;
        args[i].suffix = suffix;
        args[i].id =  i;
        args[i].repo = &repo;
        pthread_create(&tids[i], NULL, fileThreadFunction,&args[i]);
    }
    
    //for (int j=0; j < directoryThreads; ++j) 
    int j=0;
    for (; j < collection_thread_count; ++j) 
    {
		pthread_join(tids[j], NULL);
	}


    
    //sleep(5);
    
    queue_close(&directoryQueue);
	queue_close(&fileQueue);
	//printf("[ ] Queue closed\n");

    //struct node* test=NULL;
    //test = wfd("poo.txt");

    for (; i < collection_thread_count+analysisThreads; ++i) 
    {
		//pthread_join(tids[i], NULL);
	}

    for (; j < collection_thread_count+analysisThreads; ++j) 
    {
		//pthread_join(tids[j], NULL);
	}







    free(args); //remove
    free_wfd_repo(&repo);
    puts(suffix);
    free(tids);
    //free(args);
    free(suffix);
    queue_destroy(&directoryQueue);
    queue_destroy(&fileQueue);
    return EXIT_SUCCESS;

    //close queue function

}