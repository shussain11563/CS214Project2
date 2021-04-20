struct targs {
	Queue* directoryQueue;
    Queue* fileQueue;
    wfdRepo* repo;
    char* suffix;
    int id;
};

struct analysis_args
{
    size_t threadNumber;
    size_t totalThreads;
    struct node** wfdArray;
    size_t wfdArray_size;
    struct comp_result** results;
    int* occupied;
    pthread_mutex_t* lock;

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
