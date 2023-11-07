#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

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
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 6000) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server Listening\n");
    struct pollfd pollFD[4000];
    int count=1;

    pollFD[0].fd=server_socket;
    pollFD[0].events=POLLIN;

    while(1){
        int pol=poll(pollFD, count, -1);
        if(pol<0){
            perror("Poll unsuccessful");
            return -1;
        }

        for(int i=0; i<count; i++){
            if(pollFD[i].revents & POLLIN){
                if(pollFD[i].fd==server_socket){
                    int k = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
                    if(k < 0){
                        perror("Accept failed");
                        return -1;
                    }
                    pollFD[count].fd=k;
                    pollFD[count].events=POLLIN;
                    count++;
                }else{
                    char buffer[1000];
                    memset(&buffer, '\0', 1000);
                    int readr=read(pollFD[i].fd, buffer, 1000);

                    if(readr<=0){
                        break;
                    }
                    else{
                        int valRead=atoi(buffer);
                        getpeername(i, (struct sockaddr*)&client_addr, &client_len);
                        char buffer_send[1000];
                        uint64_t ans=fact(valRead);

                        snprintf(buffer_send, 1000, "%ld", ans);
                        send(pollFD[i].fd, buffer_send, 1000, 0);
                    }
                }
            }
        }

    }
    return 0;
}
