#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>

typedef struct node
{
    int data;
    struct node *next;
}NODE;

NODE*head=NULL;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *producer(void*arg)
{
    NODE*pNode=NULL;
    while(1)
    {
        pNode=(NODE*)malloc(sizeof(NODE));
        if(pNode==NULL)
        {
            exit(1);
        }
        pNode->data=rand()%1000;
        //加锁
        pthread_mutex_lock(&mutex);

        pNode->next=head;
        head=pNode;
        printf("p:[%d]\n",head->data);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        usleep(100);
    }

}
void *consumer(void*arg)
{
    NODE*pNode=NULL;
    while(1)
    {
        pthread_mutex_lock(&mutex);

        if(head==NULL)
        {
            pthread_cond_wait(&cond,&mutex);
        }
        if(head==NULL)
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }
        printf("c:[%d]\n",head->data);
        pNode=head;
        head=head->next;

        pthread_mutex_unlock(&mutex);
        free(pNode);
        pNode=NULL;
        usleep(100);
    }
}

int main()
{
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    int i=0;
    int ret;
    pthread_t thread1[3];
    for(i=0;i<3;i++)
    {
        ret=pthread_create(&thread1[i],NULL,producer,NULL);
        if(ret!=0)
        {
            printf("create error,[%s]\n",strerror(ret));
            return -1;
        }
    }
    pthread_t thread2[5];
    for(i=0;i<5;i++)
    {
        ret=pthread_create(&thread2[i],NULL,consumer,NULL);
        if(ret!=0)
        {
            printf("create error,[%s]\n",strerror(ret));
            return -1;
        }
    }
    for(i=0;i<3;i++)
    {
        pthread_join(thread1[i],NULL);
    }
    for(i=0;i<5;i++)
    {
        pthread_join(thread2[i],NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}
