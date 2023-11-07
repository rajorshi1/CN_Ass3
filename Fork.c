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

#define MAX 5000

uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

void handle_client(int client_socket) {
    uint64_t n;
    read(client_socket, &n, sizeof(uint64_t));
    if (n > 20) {
        n = 20;
    }
    uint64_t result = fact(n);
    write(client_socket, &result, sizeof(uint64_t));
    close(client_socket);
}

int main(int argc, char *argv[]) {
    int requests = argv[1];
    int server_socket, client_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server,client;
    socklen_t client_len = sizeof(client);
    server.sin_family=AF_INET;
    server.sin_port=htons(9000);
    server.sin_addr.s_addr=INADDR_ANY;

    if (!bind(server_socket,(struct sockaddr*)&server, sizeof(struct sockaddr_in)));
    else perror("Bind failed");
    if (!listen(server_socket,MAX));
    else perror("Listen failed");

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len);
        if (client_socket < 0) {
            perror("Error accepting connection");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        if (!fork()) {
            for(int i=0;i<requests;i++){
                uint64_t n;
                read(client_socket, &n, sizeof(uint64_t));
                if (n > 20) {
                    n = 20;
                }
                uint64_t result = fact(n);
                write(client_socket, &result, sizeof(uint64_t));
                close(client_socket);
                kill(getpid(),SIGKILL);
            }
        }
        else perror("Fork failed");
    }
    close(server_socket);
}
