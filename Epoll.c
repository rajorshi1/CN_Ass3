#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>


uint64_t fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int epoll_fd, nfds;
    struct epoll_event event, events[1000];

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

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Error creating epoll file descriptor");
        exit(1);
    }

    event.events = EPOLLIN;
    event.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("Error adding server socket to epoll");
        exit(1);
    }

    while (1) {
        nfds = epoll_wait(epoll_fd, events, 1000, -1);
        if (nfds == -1) {
            perror("Error in epoll_wait");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_socket) {
                client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
                if (client_socket == -1) {
                    perror("Error accepting client connection");
                } else {
                    set_nonblocking(client_socket);
                    event.events = EPOLLIN;
                    event.data.fd = client_socket;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
                        perror("Error adding client socket to epoll");
                    }
                }
            } else {
                client_socket = events[i].data.fd;
                char buffer[1024];
                int n = read(client_socket, buffer, sizeof(buffer));
                if (n <= 0) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    close(client_socket);
                } 
                else {
                    buffer[n] = '\0';
                    int num  = atoi(buffer);
                    if (num > 20) num = 20;
                    uint64_t result = fact(num);
                    char response[1024];
                    snprintf(response, sizeof(response), "Answer: %ld\n", result);
                    write(client_socket, response, sizeof(response));
                }
            }
        }
    }

    // Don't close the server socket here
    return 0;
}
