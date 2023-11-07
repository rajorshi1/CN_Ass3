#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>

#define MAX_CLIENTS 4000
#define PORT 9000

uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

int main() {
    struct sockaddr_in server_addr;
    int server_socket, client_sockets[MAX_CLIENTS];
    struct pollfd poll_fds[MAX_CLIENTS];
    int num_clients = 0;

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

    // Initialize the poll_fds for server socket
    poll_fds[0].fd = server_socket;
    poll_fds[0].events = POLLIN;
    num_clients++;

    while (1) {
        int poll_result = poll(poll_fds, num_clients, -1); // Wait indefinitely for events

        if (poll_fds[0].revents & POLLIN) {
            if (num_clients < MAX_CLIENTS) {
                // Accept a new client connection
                client_sockets[num_clients - 1] = accept(server_socket, NULL, NULL);
                if (client_sockets[num_clients - 1] != -1) {
                    poll_fds[num_clients].fd = client_sockets[num_clients - 1];
                    poll_fds[num_clients].events = POLLIN;
                    num_clients++;
                }
            }
        }

        char buffer[1024];
        for (int i = 1; i < num_clients; i++) {
        if (poll_fds[i].revents & POLLIN) {
            int n = read(poll_fds[i].fd, buffer, sizeof(buffer));
            if (n <= 0) {
                poll_fds[i] = poll_fds[num_clients - 1];
                num_clients--;
            } 
            else {
                buffer[n] = '\0';
	            int num  = atoi(buffer);
	            if (num > 20) num = 20;
	            uint64_t result = fact(num);
	            char response[1024];
	            snprintf(response, sizeof(response), "Answer: %ld\n", result);
	            write(poll_fds[i].fd, response, sizeof(response));
            }
        }
    }
    }
    close(server_socket);
    return 0;
}
