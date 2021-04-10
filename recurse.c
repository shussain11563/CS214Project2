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





    /*
    int i = 0;
    int j = 0;

    while(file[i]!='\0' || j==(strlen(suffix)-1)) //can remove j== and just return from inside while loop
    {
        if(file[i]==suffix[j])
        {
            j++;
        }
        i++;
    }

    if(j==(strlen(suffix)))
    {
        condition=1;

    }

    return condition;
    
}
*/


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
    //if(isDir(argv[1])==1)
    //{
        //printDir(argv[1], NULL);
    //}

    //char* file = "hello.txt";
    //char* suffix = ".txt";
    if(isSuffix(argv[1],argv[2])==1)
    {
        puts("Same");
    }
    else
    {
        puts("Different");
    }
}
