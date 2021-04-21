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

int counterforanalysis = 0;

// elsewhere: int sort_comps(void *r1, void *r2);
double sort_comps(void *p, void *q)
{
    // qsort(results, (repo.count*(repo.count-1))/2, sizeof(struct comp_result*), sort_comps);

    //struct comp_result* l= (((struct comp_result* )p));
    struct comp_result* l= *(struct comp_result **)p;
    struct comp_result* r= *(struct comp_result **)q;
    //struct comp_result* r= (((struct comp_result *)q)); 
    return (r->tokens - l->tokens);
    //(//struct s)*r1 > r2->tokens
}

void* analysis(void *A)
{
    struct analysis_args* a_args = A;


    for(int i = a_args->threadNumber; i < a_args->wfdArray_size; i=i+a_args->totalThreads)
    {
        for(int j = i+1; j < a_args->wfdArray_size; j++)
        {
            //printf("Pairs (%d,%d)\n", i,j);
            struct node* ptr1 = a_args->wfdArray[i];
            struct node* ptr2 = a_args->wfdArray[j];

            //printf("The filename pair is %s %s \n", ptr1->fileName, ptr2->fileName);

            struct comp_result* temp = malloc(sizeof(struct comp_result));
            
            temp->file1 = malloc(strlen(ptr1->fileName)+1);
            temp->file2 = malloc(strlen(ptr2->fileName)+1);
            //temp->file2 = malloc(strlen("word2")+1);


            char word1[strlen(ptr1->fileName)+1];
            char word2[strlen(ptr2->fileName)+1];
            strcpy(word1, ptr1->fileName);
            strcpy(word2, ptr2->fileName);

            strcpy(temp->file1, word1);
            strcpy(temp->file2, word2);

            
            //strcpy(temp->file1, "word1");
            //strcpy(temp->file2, "word2");

            temp->tokens = ptr1->numOfWords + ptr2->numOfWords;
            temp->distance = jsd(ptr1, ptr2);
            

            pthread_mutex_lock(a_args->lock);
            //printf("Inserted into positon %d \n", a_args->occupied);
            //printf("Inserted into positon %d \n", counterforanalysis);
            a_args->results[counterforanalysis] = temp;
            counterforanalysis++;
            //int i = *a_args->occupied;
            //*a_args->occupied = i++;
            //a_args->results[*a_args->occupied] = temp;
            //int i = *a_args->occupied;
            //*a_args->occupied = i++;
            pthread_mutex_unlock(a_args->lock);
     
        }
    }
/*
    //we take information on what thread is running
    //we take info on how many threads there are 

    

    struct comp_result {
    char *file1, *file2;
    unsigned tokens;     // word count of file 1 + file 2
    double distance;     // JSD between file 1 and file 2
    };

            a_args[i].threadNumber = i;
        a_args[i].totalThreads = analysisThreads;
        a_args[i].wfdArray = arr;
        a_args[i].results=results;
        wfdArray_size

        */

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


void* fileThreadFunction(void *A)
{

    struct targs* args = A;
 

    char* filename =  queue_dequeue_file(args->fileQueue, args->directoryQueue);

    while(filename)
    {
        
        char temp[strlen(filename)+1];
        strcpy(temp, filename);

        int file = open(temp, O_RDONLY);
        //truct node* test = wfd(file,temp);
        struct node* test = wfd(file,temp);

        close(file);

        wfd_repo_insert(args->repo, test);

        //puts(filename);
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
    pthread_t* tids = malloc(sizeof(pthread_t) * collection_thread_count);
    struct targs* args = malloc(sizeof(struct targs) * collection_thread_count);


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
    
    int j=0;
    for (; j < collection_thread_count; ++j) 
    {
		pthread_join(tids[j], NULL);
	}


    

    
    queue_close(&directoryQueue);
	queue_close(&fileQueue);
	//printf("[ ] Queue closed\n");







    
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
    //wfdRepoNode* prev = NULL;
    //puts("This is the filename");
    while(ptr!=NULL)
    {
        arr[k]=ptr->data;
        struct node* temp = arr[k];
        //puts(temp->fileName);
        //puts(ptr->data->fileName);
        ptr = ptr->next;
        k++;
    }



    //create array of structs
   
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


        //a_args[i].

        //pass in wfdRepo Array vesion
        //pass in comp_rsults array


        pthread_create(&a_tids[i], NULL, analysis,&a_args[i]);
		////pthread_join(tids[i], NULL);
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
        //free(results[i]);
    } 

    free(occupied);
    pthread_mutex_destroy(&lock);
    free(results);
    free(a_args);
    free(args);
    free(arr); //remove
    free_wfd_repo(&repo);
    //puts(suffix);
    free(tids);
    free(a_tids);
    //free(args);
    free(suffix);
    queue_destroy(&directoryQueue);
    queue_destroy(&fileQueue);
    return EXIT_SUCCESS;

    //close queue function

}