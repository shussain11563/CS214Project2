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

struct dt_args {
	Queue* directoryQueue;
    Queue* fileQueue;
    char* suffix;
};

int isFile(char *filename)
{
    struct stat meta_data;
    int status = stat(filename, &meta_data);
    if(status==-1)
    {
        //perror("File Does Not Exist"); 
        return -1;
    }

    if(S_ISREG(meta_data.st_mode))
    {
        return 1;
    }

    return 0;
}

int isDir(char *filename)
{
    if(strlen(filename)>=1)
    {
        if(filename[0]=='.')
        {
            return 0;
        }
        
    }
    struct stat meta_data;
    int status = stat(filename, &meta_data);
    if(status==-1)
    {
        //perror("File Does Not Exist"); 
        return -1;
    }

    if(S_ISDIR(meta_data.st_mode))
    {
        return 1;
    }

    return 0;
}

char* generateFilePath(char* directoryName, char* filePath)
{
    strbuf_t path;
    sb_init(&path, 10);
    sb_concat(&path, directoryName);
    sb_append(&path, '/');

    sb_concat(&path, filePath);

    char* ret = malloc(sizeof(char)* (path.used+1));
    strcpy(ret, path.data);
    
    sb_destroy(&path);
    return ret;
}

int isSuffix(char* file, char* suffix)
{
    int condition = 0;
    if(strlen(file)<strlen(suffix))
    {
        return condition;
    }

    int j = strlen(suffix);

    int i =0;
    while(j!=0)
    {
        if((file[strlen(file)-1-i])!=(suffix[strlen(suffix)-1-i]))
        {
            return condition;
        }
        i++;
        j--;
    }

    condition =1;
    return condition;
} 


int threadCount(char* argument, char* flag)
{
    int threadCount = 1;
    if(strlen(argument)==strlen(flag))
    {
        return threadCount;  //defaultNumber ??
    }

    int i = strlen(flag);
    //check if it is a valid 
    while(argument[i]!='\0')
    {
        if(!(isdigit(argument[i])))
        {
            threadCount = -1;  //not valid --- return -1 and throw error;
            return threadCount;
        }
        i++;
    }

    //parse integer   ----> use memcpy or memmove to condense code
    i = strlen(flag);
    strbuf_t grabInteger;
    sb_init(&grabInteger, 10);

    while(argument[i]!='\0')
    {
        sb_append(&grabInteger, argument[i]);
        i++;
    }

    threadCount = atoi(grabInteger.data);
    
    sb_destroy(&grabInteger);
    return threadCount;
}

char* stringParser(char* argument, char* flag)
{
    if(strlen(argument)==strlen(flag))
    {
        //return threadCount;  //defaultNumber ??
    }

    //parse string   ----> use memcpy or memmove to condense code
    int i = strlen(flag);
    strbuf_t suffix;
    sb_init(&suffix, 10);

    while(argument[i]!='\0')
    {
        sb_append(&suffix, argument[i]);
        i++;
    }
    char* ret = malloc(sizeof(char)*(suffix.used+1));
    strcpy(ret, suffix.data);
    
    sb_destroy(&suffix);
    return ret;
}

void* fileThreadFunction(void *A)
{
    printf("Hello Directory\n");
    sleep(3);
    printf("Goodbye\n");
}

//directory thread
void* directThreadFunction(void *A)
{
    struct dt_args* args = A;
    while(args->directoryQueue->head!=NULL)
    {
        char* filename =  queue_dequeue(args->directoryQueue);
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
                puts("File:");
                puts(comboString);
                queue_insert(args->fileQueue,temp);
            }
            else if(isDir(comboString)==1)
            {   
                puts("Directory:");
                puts(comboString);
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
void Files(Queue* fileQueue, char* suffix)
{
    char* filename = queue_dequeue(fileQueue);

    //wip
    free(filename);
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
    struct dt_args* args;

    args.directoryQueue = &directoryQueue;
    args.fileQueue = &fileQueue;
    args.suffix = suffix;


    int i = 0;
    for(; i <directoryThreads; i++)
    {
        pthread_create(&tids[i], NULL, directThreadFunction, &args);
    }
    for(; i <threads; i++)
    {
        //pthread_create(&tids[i], NULL, fileThreadFunction,NULL);
    }

    sleep(5);
    //for(; i <threads; i++)
    //{
        //pthread_create(&tids[i])
    //}
    for (i = 0; i < directoryThreads; ++i) 
    {
		pthread_join(tids[i], NULL);
	}

    





    puts(suffix);
    free(tids);
    //free(args);
    free(suffix);
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