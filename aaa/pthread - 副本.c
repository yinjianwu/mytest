#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <pthread.h>
#include "wrap.h"

struct INFO
{
    pthread_t thread;
    int cfd;
    int idx;
    struct sockaddr_in client;

}info[1024];

void *mythread(void *arg)
{
    int i;
    int n;
    socklen_t len;
    char buf[1024];
    char sIP[16];
    struct INFO *pInfo = (struct INFO *)arg;
    int cfd = pInfo->cfd;
    printf("client: IP[%s]-[%d]\n", inet_ntop(AF_INET, &pInfo->client.sin_addr, sIP, sizeof(sIP)), ntohs(pInfo->client.sin_port));

    printf("idx==[%d]\n", pInfo->idx);

    while(1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = Read(cfd, buf, sizeof(buf));
        if(n<=0)
        {
            printf("read error or client closed, n==[%d]\n", n);
            Close(cfd);
            pInfo->cfd = -1;
            break;
        }
        printf("[%d]:n==[%d]:buf==[%s]\n", *(int *)arg, n, buf);

            if(strcmp(buf,"HELLO"))
        {
            write(cfd,"world",5);
        }
        else if(strcmp(buf,"WORLD"))
        {
            write(cfd,"hello",5);
        }
        else
        {
            for(i=0; i<n; i++)
            {
                buf[i] = toupper(buf[i]);	
            }	

            Write(cfd, buf, n);
        }
    }

    pthread_exit(NULL);
}

int findIndex()
{
    int i = 0;
    for(i=0; i<1024; i++)
    {
        if(info[i].cfd==-1)
        {
            return i;
        }
    }

    if(i==1024)
    {
        return -1;
    }

}

int main()
{
    int lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));
    Listen(lfd, 128);

    int cfd;
    int n;
    pid_t pid;
    socklen_t len;
    char buf[1024];
    struct sockaddr_in client;

    int ret;
    int i = 0;
    for(i=0; i<1024; i++)
    {
        info[i].cfd = -1;	
    }

    int idx;
    while(1)
    {
        bzero(&client, sizeof(client));

        cfd = Accept(lfd, (struct sockaddr *)&client, &len);

        idx = findIndex();	
        if(idx==-1)
        {
            sleep(1);
            continue;
        }

        info[idx].cfd =cfd;
        info[idx].idx = idx;
        memcpy(&info[idx].client, &client, sizeof(client));

        ret = pthread_create(&info[idx].thread, NULL, mythread, (void *)&info[idx]);	
        if(ret!=0)
        {
            printf("create thread error, [%s]\n", strerror(ret));
            Close(lfd);
            exit(0);
        }

        pthread_detach(info[idx].thread);
        i++;
    }

    return 0;
}
