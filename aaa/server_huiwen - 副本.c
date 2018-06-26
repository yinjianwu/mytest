#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int huiwen(char*buf)
{
    char *ftemp=buf;
    char *ptemp=buf+strlen(buf)-2;
    
    while(ftemp<ptemp)
    {
        if(*ftemp!=*ptemp)
        {
            return 1;
        }
        ftemp++;
        ptemp--;
    }

    return 0;
}
int main()
{
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    if(lfd<0)
    {
        perror("lfd error");
        return -1;
    }
    struct sockaddr_in sev;
    bzero(&sev,sizeof(sev));
    sev.sin_family =AF_INET;
    sev.sin_port=htons(8888);
    sev.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(lfd,(struct sockaddr*)&sev,sizeof(sev));

    listen(lfd,5);
    socklen_t len ;
    struct sockaddr_in client;
    memset(&client,0x00,sizeof(client));
    len=sizeof(client);
    int cfd=accept(lfd,(struct sockaddr*)&client,&len);
    if(cfd<0)
    {

        perror("accpet error");
        return -1;
    }
    int i;
    int n;
    int ret;
    char  buf[1024];
    while (1)
    {
        memset(buf,0x00,sizeof(buf));
         n=read(cfd,buf,sizeof(buf));
        if(n<=0)
        {
            printf("read error or client close,n==[%d]\n",n);
            break;
        }
        printf("n==[%d],buf==[%s]\n",n,buf);
        
        ret=huiwen(buf);
        if(!ret)
        {
            printf("相同\n");
        }
        else
        {
            printf("不相同\n");
        }
        write(cfd,buf,n);
    }
    close(lfd);
    close(cfd);
    return 0;
}
