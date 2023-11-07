#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    uint64_t n;
    read(client_socket, &n, sizeof(uint64_t));
    if (n > 20) {
        n = 20;
    }
    uint64_t result = fact(n);
    write(client_socket, &result, sizeof(uint64_t));
    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t threads[5];  // Maximum 5 concurrent threads

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        // Create a new thread to handle the client
        if (pthread_create(&threads[i++], NULL, handle_client, (void *)&client_socket) != 0) {
            perror("Error creating thread");
            close(client_socket);
        }

        if (i >= 5) {
            i = 0;  // Reset thread index
            while (i < 5) {
                pthread_join(threads[i++], NULL);  // Wait for threads to finish
            }
            i = 0;  // Reset thread index after joining threads
        }
    }

    close(server_socket);
    return 0;
}
