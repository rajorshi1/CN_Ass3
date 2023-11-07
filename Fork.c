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
            char buffer[1024];
    	    while (1) {
		int n = read(client_socket, buffer, sizeof(buffer));
		buffer[n] = '\0';
		int num  = atoi(buffer);
		if (num > 20) num = 20;
		uint64_t result = fact(num);
		char response[1024];
		snprintf(response, sizeof(response), "Answer: %ld\n", result);
		write(client_socket, &response, sizeof(response));
    		}
        }
    }
    return 0;
}
