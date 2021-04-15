
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
foo --> test  --> hello
//import suffix
void* directThreadFunction(void *A, Queue* directoryQueue, Queue* fileQueue, char* suffix)
{
    //char* filename =  queue_dequeue(directoryQueue);
    while(directoryQueue->head!=NULL)
    {
        DIR* dirp = opendir(filename);
        struct dirent* entry;

        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }
            if(isSuffix(filename,suffix)==1 && isFile(filename)==1)
            {
                //queue_insert(fileQueue,filename);
                puts(filename);
                continue;
            }
            //char* comboString = generateFilePath(file, entry->d_name);
            if(isDir(entry->d_name)==1)
            {
                puts(entry->d_name);
                //printDir(comboString, comboString);
            }
            //else
            //{   
                //puts(comboString); 
                //free(comboString);
            //}
            printf("Faied to read Directory");

        }
        closedir(dirp);
        //open function
    }

    //free(filename);
}

----------------
void directThreadFunction(char* filename)
{
    //char* filename =  queue_dequeue(directoryQueue);
    while(directoryQueue->head!=NULL)
    {
        DIR* dirp = opendir(filename);
        struct dirent* entry;

        while(entry = readdir(dirp))
        {
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name)==0)
            {
                continue;
            }
            if(isSuffix(filename,suffix)==1 && isFile(filename)==1)
            {
                //queue_insert(fileQueue,filename);
                puts(filename);
                continue;
            }
            //char* comboString = generateFilePath(file, entry->d_name);
            if(isDir(entry->d_name)==1)
            {
                puts(entry->d_name);
                //printDir(comboString, comboString);
            }
            //else
            //{   
                //puts(comboString); 
                //free(comboString);
            //}
            printf("Faied to read Directory");

        }
        closedir(dirp);
        //open function
    }

    //free(filename);
}