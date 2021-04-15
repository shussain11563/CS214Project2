#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include<dirent.h>
#include<ctype.h>
#include "strbuf.h"
#include<ctype.h>


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
        printf("The character is %c vs %c\n",file[strlen(file)-1-i],suffix[strlen(suffix)-1-i]);
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


//remove 
int isNumber(char* stringReg)
{
    int condition = 1;
    int i = 0;
    while(!(stringReg[i]=='\0'))
    {
        if(!(isdigit(stringReg[i])))
        {
            condition = 0;
            return condition;
        }
        i++;
    }
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
        //return threadCount;  //defaultNumber ?? //return -1 and return error
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


void printDir(char* file, char* ret)
{
    DIR* dirp = opendir(file);
    struct dirent* entry;

        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }

            char* comboString = generateFilePath(file, entry->d_name);
            if(isDir(comboString)==1)
            {
                printDir(comboString, comboString);
            }
            else
            {
                puts(comboString); 
                free(comboString);
            }

        }
        closedir(dirp);

    if(ret!=NULL)
    {
        free(ret);
    }
    return;

}


int main(int argc, char **argv)
{
    int directoryThreads = 1;
    int fileThreads = 1;
    int analysisThreads = 1;
    char* suffix = ".txt";

    for(int i = 1; i < argc; i++)
    {
        if(isDir(argv[i])==1)
        {
            //add to directory queue
        }
        else if(isFile(argv[i])==1)
        {
            //add to file queue 
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

    


    //if(isDir(argv[1])==1)
    //{
        //printDir(argv[1], NULL);
    //}

    //char* file = "hello.txt";
    //char* suffix = ".txt";
    /*
    if(isSuffix(argv[1],argv[2])==1)
    {
        puts("Same");
    }
    else
    {
        puts("Different");
    }
    */
}