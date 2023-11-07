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
    char buffer[1024];
    while (1) {
	int n = read(client_socket, buffer, sizeof(buffer));
	if (n<=0){
		break;
	}
	buffer[n] = '\0';
	int num  = atoi(buffer);
	if (num > 20) num = 20;
	uint64_t result = fact(num);
	char response[1024];
	snprintf(response, sizeof(response), "Answer: %ld\n", result);
	write(client_socket, &response, sizeof(response));
    }
    return NULL;
   //thread_exit(NULL);
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
    server_addr.sin_port = htons(9901);
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
    int i = 0;
    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            //perror("Error accepting connection");
            close(client_socket);
            //exit(EXIT_FAILURE);
            continue;
        }
        pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        pthread_create(&t, NULL, handle_client, pclient);
    }
    return 0;
}

