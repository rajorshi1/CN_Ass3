#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        close(server_socket);
    }

    // Listen for incoming connections
    if (listen(server_socket, 6000) == -1) {
        perror("Error listening");
        close(server_socket);
    }
    printf("Server Listening\n");
    struct epoll_event epolls[4000];
    struct epoll_event event;
    int eval =epoll_create(4000);
    if(eval<0){
        perror("Epoll not created");
        return -1;
    }
    event.events=EPOLLIN;
    event.data.fd=server_socket;
    int check = epoll_ctl(eval, EPOLL_CTL_ADD, server_socket, &event);
    if(check<0){
        perror("Epoll Control Failed");
        return -1;
    }
    while(1){
        int num_events = epoll_wait(eval, epolls, 4000, -1);
        if(num_events < 0){
            perror("Epoll Failed");
            return -1;
        }

        for(int i=0; i<num_events; i++){
            if(epolls[i].events & EPOLLIN){
                if(epolls[i].data.fd==server_socket){
                    int k = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
                    if(k<0){
                        perror("Accept error");
                        return -1;
                    }
                    event.events=EPOLLIN;
                    event.data.fd=k;
                    if (epoll_ctl(eval, EPOLL_CTL_ADD, k, &event) < 0){
                        perror("Epoll Control Failed");
                        exit(1);
                    }
                }
                else{
                    char buffer[1000];
                    memset(&buffer, '\0', 1000);
                    int n = read(epolls[i].data.fd, buffer, sizeof(buffer));
                    if(n <= 0){
                        break;
                    }
                    else{
                        int valRead=atoi(buffer);
                        getpeername(i, (struct sockaddr*)&client_addr, &client_len);
                        char buffer_send[1000];
                        uint64_t ans=fact(valRead);

                        snprintf(buffer_send, 1000, "%ld", ans);
                        send(epolls[i].data.fd, buffer_send, 1000, 0);
                    }
            	}
        }
    	}
    }
    return 0;
}
