#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <signal.h>

#define CONTENT_SIZE 3000
#define MAX 5000
#define min(a,b) ((a)<(b)?(a):(b))

uint64_t fact(uint64_t n){
    uint64_t result=1;
    int high=min(n,20);
    for(int i=1;i<=high;i++){
        result*=i;
    }
    return result;
}

int main(){
    int requests;
    printf("Enter the number of requests: ");
    scanf("%d",&requests);
    int sockfd;
    int opt;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        perror("Socket creation failed");
    }
    struct sockaddr_in server,client;
    server.sin_family=AF_INET;
    server.sin_port=htons(9000);
    server.sin_addr.s_addr=INADDR_ANY;
    int len=sizeof(struct sockaddr_in);
    int setsock=setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
    if(setsock<0){
        perror("Setsockopt failed");
    }
    int bindsock=bind(sockfd,(struct sockaddr*)&server,len);
    if(bindsock<0){
        perror("Bind failed");
    }
    int listensock=listen(sockfd,MAX);
    if(listensock<0){
        perror("Listen failed");
    }
    int newsockfd;
    pid_t pid;
    while(true){
        newsockfd=accept(sockfd,(struct sockaddr*)&client,&len);
        if(newsockfd<0){
            perror("Accept failed");
        }
        pid=fork();
        if(pid<0){
            perror("Fork failed");
        }
        if(pid==0){
            for(int i=0;i<requests;i++){
                char buffer[CONTENT_SIZE];
                recv(newsockfd,&buffer,sizeof(buffer),0);
                uint64_t n=strtoull(buffer,NULL,10);
                uint64_t result=fact(n);
                send(newsockfd,&result,sizeof(result),0);
            }
            close(newsockfd);
            kill(getpid(),SIGKILL);
        }
    }
    close(sockfd);
    close(newsockfd);
    return 0;
}
