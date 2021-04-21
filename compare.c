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

//global variable used to keep track for analysis threads
int counterforanalysis = 0;

//comparison operator for qsort
int sort_comps(void *p, void *q)
{
    struct comp_result* l= *(struct comp_result **)p;
    struct comp_result* r= *(struct comp_result **)q;
    return (r->tokens - l->tokens);
}

//analysis thread 
void* analysis(void *A)
{
    struct analysis_args* a_args = A;
    for(int i = a_args->threadNumber; i < a_args->wfdArray_size; i=i+a_args->totalThreads)
    {
        for(int j = i+1; j < a_args->wfdArray_size; j++)
        {
            struct node* ptr1 = a_args->wfdArray[i];
            struct node* ptr2 = a_args->wfdArray[j];

            struct comp_result* temp = malloc(sizeof(struct comp_result));
            
            temp->file1 = malloc(strlen(ptr1->fileName)+1);
            temp->file2 = malloc(strlen(ptr2->fileName)+1);

            char word1[strlen(ptr1->fileName)+1];
            char word2[strlen(ptr2->fileName)+1];
            strcpy(word1, ptr1->fileName);
            strcpy(word2, ptr2->fileName);

            strcpy(temp->file1, word1);
            strcpy(temp->file2, word2);

            temp->tokens = ptr1->numOfWords + ptr2->numOfWords;
            temp->distance = jsd(ptr1, ptr2);

            pthread_mutex_lock(a_args->lock);
            a_args->results[counterforanalysis] = temp;
            counterforanalysis++;
            pthread_mutex_unlock(a_args->lock);
     
        }
    }
    return;
}

//directory thread
void* directThreadFunction(void *A)
{
    struct targs* args = A;
    
    pthread_mutex_lock(&args->directoryQueue->lock);
    args->directoryQueue->activeThread++;
    pthread_mutex_unlock(&args->directoryQueue->lock);

    char* filename = queue_dequeue_dir(args->directoryQueue, args->fileQueue);
    while(filename)
    {
    
        if(filename==NULL)
        {
            return;
        }
      
        DIR* dirp = opendir(filename);
       
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
                queue_insert(args->fileQueue,temp);
            }
            else if(isDir(comboString)==1)
            {  
                queue_insert(args->directoryQueue,temp);
            }
            free(comboString);
        }
        
        closedir(dirp);
        free(filename);
        filename =  queue_dequeue_dir(args->directoryQueue, args->fileQueue);
    }
    return;
}

//fileThreadFunction
void* fileThreadFunction(void *A)
{
    struct targs* args = A;
    char* filename =  queue_dequeue_file(args->fileQueue, args->directoryQueue);

    while(filename)
    {
        
        char temp[strlen(filename)+1];
        strcpy(temp, filename);

        int file = open(temp, O_RDONLY);
        struct node* test = wfd(file,temp);

        close(file);

        wfd_repo_insert(args->repo, test);

        free(filename);
        filename =  queue_dequeue_file(args->fileQueue, args->directoryQueue);
    }
    return;
}

int main(int argc, char* argv[])
{
    int directoryThreads = 1;
    int fileThreads = 1;
    int analysisThreads = 1;
    int collection_thread_count;
    char* suffix = NULL;
    Queue directoryQueue;
    Queue fileQueue;
    wfdRepo repo;
   
    //initalized queues
    queue_init(&directoryQueue);
    queue_init(&fileQueue);
    wfd_repo_init(&repo);

    //grabs arguments
    for(int i = 1; i < argc; i++)
    {
        if(isDir(argv[i])==1)
        {
            queue_insert(&directoryQueue, argv[i]);
        }
        else if(isFile(argv[i])==1)
        {
            queue_insert(&fileQueue, argv[i]);
        }
        else if(strncmp(argv[i], "-d", 2)==0)
        {
            directoryThreads = threadCount(argv[i], "-d");
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
            suffix = stringParser(argv[i], "-s");
        }
        else
        {
            //ignore 
        }
    }
    if(suffix==NULL)
    {
        char* temp = ".txt";
        suffix = malloc(sizeof(char)*(strlen(temp)+1));
        strcpy(suffix, temp);
    } 
    collection_thread_count = directoryThreads+fileThreads; 
    pthread_t* tids = malloc(sizeof(pthread_t) * collection_thread_count);
    struct targs* args = malloc(sizeof(struct targs) * collection_thread_count);

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

    for(; i <collection_thread_count; i++)
    {
        args[i].directoryQueue = &directoryQueue;
        args[i].fileQueue = &fileQueue;
        args[i].suffix = suffix;
        args[i].id =  i;
        args[i].repo = &repo;
        pthread_create(&tids[i], NULL, fileThreadFunction,&args[i]);
    }
    
    int j=0;
    for (; j < collection_thread_count; ++j) 
    {
		pthread_join(tids[j], NULL);
	}

    queue_close(&directoryQueue);
	queue_close(&fileQueue);

    
    pthread_t* a_tids = malloc(sizeof(pthread_t) * analysisThreads);
    struct analysis_args* a_args = malloc(sizeof(struct analysis_args) * analysisThreads);
    struct node** arr = malloc(sizeof(struct node*)*repo.count);
    struct comp_result** results = malloc(sizeof(struct comp_result*) * ((repo.count*(repo.count-1))/2));
    int *occupied = malloc(sizeof(int));
    *occupied =0;
    for(int i = 0; i <(repo.count*(repo.count-1))/2; i++)
    {
        results[i] = NULL;

    } 

    wfdRepoNode* ptr = repo.head;
    int k = 0;
    while(ptr!=NULL)
    {
        arr[k]=ptr->data;
        struct node* temp = arr[k];
        ptr = ptr->next;
        k++;
    }

   
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL); 
    
    for (int i=0; i < analysisThreads; ++i) 
    {
        a_args[i].threadNumber = i;
        a_args[i].totalThreads = analysisThreads;
        a_args[i].wfdArray = arr;
        a_args[i].results=results;
        a_args[i].wfdArray_size=repo.count;
        a_args[i].lock = &lock;
        a_args[i].occupied = occupied;
        pthread_create(&a_tids[i], NULL, analysis,&a_args[i]);
	}

    for (int j=0; j < analysisThreads; ++j) 
    {
		pthread_join(a_tids[j], NULL);
	}

    qsort(results, (repo.count*(repo.count-1))/2, sizeof(struct comp_result**), sort_comps);
        
    for(int i = 0; i <(repo.count*(repo.count-1))/2; i++)
    {
        struct comp_result* temp =  results[i];
        printf("%f %s %s\n", temp->distance, temp->file1, temp->file2);
        free(temp->file1);
        free(temp->file2);
        free(temp);
    } 


    //frees our memory allocated structures and queues, destroys the analysis thread lock, and closes argument structs
    free(occupied);
    pthread_mutex_destroy(&lock);
    free(results);
    free(a_args);
    free(args);
    free(arr); 
    free_wfd_repo(&repo);
    free(tids);
    free(a_tids);
    free(suffix);
    queue_destroy(&directoryQueue);
    queue_destroy(&fileQueue);
    return EXIT_SUCCESS;
}