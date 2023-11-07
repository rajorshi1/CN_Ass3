#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>

uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

int main() {
    int socket_server, client_server;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9901);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket
    if (bind(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        close(socket_server);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(socket_server, 6000) == -1) {
        perror("Error listening");
        close(socket_server);
        exit(EXIT_FAILURE);
    }
    printf("Server Listening\n");

    fd_set read_fds, client_fds;
    int max_fd = socket_server;

    FD_ZERO(&client_fds);
    FD_SET(socket_server, &client_fds);

    while (true) {
        read_fds = client_fds;
        int num_ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == socket_server) {
                    client_server = accept(socket_server, (struct sockaddr*)&client_addr, &len);
                    if (client_server < 0) {
                        perror("Accept failed");
                    } else {
                        FD_SET(client_server, &client_fds);
                        if (client_server > max_fd) {
                            max_fd = client_server;
                        }
                    }
                } else {
                    for (int j = 0; j < 50; j++) {
                        char buffer[1024];
                        int n = read(i, buffer, sizeof(buffer));
                        if (n <= 0) {
                            close(i);
                            FD_CLR(i, &client_fds);
                            break;
                        }
                        buffer[n] = '\0';
                        uint64_t num = atoi(buffer);
                        uint64_t result = fact(num);
                        char response[1024];
	                    snprintf(response, sizeof(response), "Answer: %ld\n", result);
	                    write(i, &response, sizeof(response));
                    }
                }
            }
        }
    }

    return 0;
}
