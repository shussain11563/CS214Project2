#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include <string.h>


struct node* wfd(int file);
double jsd(struct node* list1, struct node* list2);

struct node{
    int count;
    int numOfWords;
    double frequency;
    double meanFreq;
    char* word;
    struct node* next;
};


struct node* insert(struct node* front, char* word, int count);
void freeList(struct node* front);


struct node* insert(struct node* front,  char* word, int count){
    struct node* n;
    n=(struct node*)malloc(sizeof(struct node));
    if(!n){
        puts("Out of memory");
        exit(EXIT_FAILURE);
    }
	n->count=count;
    n->numOfWords=0;
    n->meanFreq=0;
	n->frequency=0;
    n->word=malloc(strlen(word)+1);
    if(!n->word){
        puts("Out of memory");
        exit(EXIT_FAILURE);
    }
    strcpy(n->word,word);
    n->next=NULL;
	struct node* ptr = front;

    if(front==NULL){//first item
        n->numOfWords=1;
        return n;
    }
    
    if(strcmp(n->word,front->word)<0){//if we need to make a new front
        n->numOfWords=++front->numOfWords;
        n->next=front;
        return n;
    }

    while(ptr->next!=NULL){
        if(strcmp(ptr->word,n->word)<0&&strcmp(ptr->next->word,n->word)>0){//if the word should be after the one we are looking at, and before the next one
            n->next=ptr->next;
            ptr->next=n;
            ++front->numOfWords;
            return front;
        }
        else if(strcmp(ptr->word,n->word)==0){
            ++ptr->count;
            free(n->word);
            free(n);
            return front;
        }
        ptr=ptr->next;
    }

    if(strcmp(ptr->word,n->word)==0){
        ++ptr->count;
        free(n->word);
        free(n);
        return front;
    }

    ptr->next=n;
    ++front->numOfWords;
    return front;
}

void freeList(struct node* front){
	struct node* temp=NULL;
	while(front!=NULL){
		temp=front;
		front=front->next;
		free(temp);
	}
}

/////

struct node* wfd(int file){
    struct node* words = NULL;
    char* w;
    strbuf_t buf;
    strbuf_t temp;
    double numberOfWords=0;
    int bytes_read;
    sb_init(&buf,20);
    sb_init(&temp,20);

    bytes_read = read(file,buf.data,20);

    if(bytes_read==0){//empty file
        sb_destroy(&buf);
        sb_destroy(&temp);
        words=NULL;
        return words;
    }


    //makes lowercase
    int m;
    for(m=0;m<bytes_read;++m){
        buf.data[m]=tolower(buf.data[m]);
    }

    while(bytes_read!=0)
    {
        int startIndex=0;
        int startedWord=0;
        int wordBytes=0;
        for(int i=0;i<20;++i)
        {
            if(isalpha(buf.data[i])||isdigit(buf.data[i])||buf.data[i]=='-'||buf.data[i]=='\'')
            {//it is a letter
                if(startedWord==0)
                {
                    startedWord=1;
                    startIndex=i;
                    wordBytes=0;
                }
                wordBytes++;
            }
            else
            {//not a letter
                if(startedWord==1)
                {//end of word reached
                    startedWord=0;
                    w = (char*) malloc(sizeof(char)*wordBytes+1);
                    if(!w)
                    {
                        puts("Out of memory");
                        exit(EXIT_FAILURE);
                    }
                    strncpy(w,&buf.data[startIndex],wordBytes);
                    w[wordBytes]='\0';
                    for(m=0;m<wordBytes;++m){
                        if(w[m]=='\''){
                            memmove(&w[m],&w[m+1],strlen(w)-m);
                        }
                    }
                    numberOfWords++;
                    words = insert(words,w,1);
                    free(w);
                }
            }
        }
        if(startedWord==1){//in the middle of a word
            int x = startIndex;
            for(int j=0;j<wordBytes;++j){
                sb_append(&temp,buf.data[x]);
                ++x;
            }
        }

        sb_destroy(&buf);
        sb_init(&buf,20);
        for(int k=0;k<wordBytes;++k){
            sb_append(&buf,temp.data[k]);
        }

        bytes_read = read(file,buf.data+temp.used,20-temp.used);
        for(m=0;m<bytes_read;++m)
        {
            buf.data[m]=tolower(buf.data[m]);
        }
        sb_destroy(&temp);
        sb_init(&temp,20);
    }

    sb_destroy(&buf);
    sb_destroy(&temp);

    struct node* ptr = words;
    while(ptr!=NULL){//setting the WFD for each word in the LL
        ptr->frequency=ptr->count/numberOfWords;
        ptr=ptr->next;
    }

    return words;
}

double jsd(struct node* list1, struct node* list2){
    struct node* ptr1 = list1;
    struct node* ptr2 = list2;
    double kld1=0;
    double kld2=0;
    double jsd=0;

    while(ptr1!=NULL && ptr2!=NULL){
        if(strcmp(ptr1->word,ptr2->word)<0){
            ptr1->meanFreq=ptr1->frequency/2;
            ptr1=ptr1->next;
        }
        else if(strcmp(ptr1->word,ptr2->word)>0){
            ptr2->meanFreq=ptr2->frequency/2;
            ptr2=ptr2->next;
        }
        else{//do the maths
            ptr1->meanFreq=(ptr1->frequency+ptr2->frequency)/2;
            ptr2->meanFreq=(ptr1->frequency+ptr2->frequency)/2;
            ptr1=ptr1->next;
            ptr2=ptr2->next;
        }
    }

    if(ptr1==NULL){
        while(ptr2!=NULL){
            ptr2->meanFreq=ptr2->frequency/2;
            ptr2=ptr2->next;
        }
    }
    else if(ptr2==NULL){
        while(ptr1!=NULL){
            ptr1->meanFreq=ptr1->frequency/2;
            ptr1=ptr1->next;
        }
    }

    ptr1=list1;
    ptr2=list2;

    while(ptr1!=NULL){
        kld1+=ptr1->frequency*(log10(ptr1->frequency/ptr1->meanFreq)/log10(2));
        ptr1=ptr1->next;
    }

    while(ptr2!=NULL){
        if(ptr2->meanFreq!=0){
            kld2+=ptr2->frequency*(log10(ptr2->frequency/ptr2->meanFreq)/log10(2));            
        }
        ptr2=ptr2->next;
    }

    jsd = sqrt((kld1/2)+(kld2/2));

    return jsd;
}