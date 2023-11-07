#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the client
        if (fork() == 0) {
            close(server_socket);  // Child process doesn't need the listening socket
            handle_client(client_socket);
            exit(EXIT_SUCCESS);  // Child process exits after handling client
        } else {
            close(client_socket);  // Parent process doesn't need the client socket
        }
    }

    close(server_socket);
    return 0;
}
