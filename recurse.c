#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include<dirent.h>
#include "strbuf.h"


//update 
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
            //puts(comboString); 
            //free(comboString);

            //printf("%s, %d\n", comboString, isDir(comboString));
            //free(comboString);
            if(isDir(comboString)==1)
            {
                //puts("poop");
                
                //puts(comboString);
                //free(comboString);
                printDir(comboString, comboString);
            }
            else
            {
                puts(comboString); 
                free(comboString);
            }
            //free(comboString);
            //if directory 
            //puts(entry->d_name);
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
    if(isDir(argv[1])==1)
    {
        printDir(argv[1], NULL);
    }
    /*
    if(isDir(argv[1])==1)
    {
        printf("This is a directory!\n");
        DIR* dirp = opendir(argv[1]);

        struct dirent* entry;
        //ignore .. and .   
        
        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }
            //if directory 
            puts(entry->d_name);
        }
    }
    else
    {
        printf("This is not a directory\n");
    }
    */
}
