#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/time.h>

#define MAX_CLIENTS 4000
#define PORT 9999

// Function to compute factorial
uint64_t fact(int n) {
    if (n > 20) n = 20;
    uint64_t result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    int server_fd, new_socket, max_sd, activity;
    int client_sockets[MAX_CLIENTS];
    fd_set readfds;
    int max_clients = MAX_CLIENTS;

    // Initialize client sockets
    for (int i = 0; i < max_clients; ++i) {
        client_sockets[i] = 0;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 6000) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (int i = 0; i < max_clients; ++i) {
            int sd = client_sockets[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for an activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select error");
        }

        // If something happened on the server socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // Add new socket to the list of client sockets
            for (int i = 0; i < max_clients; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Check client sockets for data to read
        for (int i = 0; i < max_clients; ++i) {
            int sd = client_sockets[i];
            char buffer[1024];
            if (FD_ISSET(sd, &readfds)) {
                int n;
                n = read(sd, buffer, sizeof(buffer)); 
                if (n <= 0) {
                    // Client disconnected, remove the socket
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    // Read payload and compute factorial
                    buffer[n] = '\0';
                    int num = atoi(buffer);
                    uint64_t result = fact(num);

                    // Send the factorial result back to the client
                    char response[1024];
                    snprintf(response, sizeof(response), "%ld\n", result);
                    write(sd, response, strlen(response));
                }
            }
        }
    }

    return 0;
}
