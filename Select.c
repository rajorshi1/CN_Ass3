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

uint64_t fact(uint64_t n) {
    uint64_t result = 1;
    if (n > 20) n = 20;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port = htons(9900);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    int len = sizeof(struct sockaddr_in);

    int bindsock = bind(sockfd, (struct sockaddr*)&server, len);
    if (bindsock < 0) {
        perror("Bind failed");
        exit(1);
    }

    int listensock = listen(sockfd, 6000);
    if (listensock < 0) {
        perror("Listen failed");
        exit(1);
    }

    int newsockfd;
    fd_set read_fds, master_fds;
    int max_fd = sockfd;

    FD_ZERO(&master_fds);
    FD_SET(sockfd, &master_fds);

    while (true) {
        read_fds = master_fds;
        int num_ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (num_ready < 0) {
            perror("Select failed");
            exit(1);
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == sockfd) {
                    newsockfd = accept(sockfd, (struct sockaddr*)&client, &len);
                    if (newsockfd < 0) {
                        perror("Accept failed");
                    } else {
                        FD_SET(newsockfd, &master_fds);
                        if (newsockfd > max_fd) {
                            max_fd = newsockfd;
                        }
                    }
                } else {
                    for (int j = 0; j < 50; j++) {
                        char buffer[1024];
                        ssize_t bytes_received = recv(i, buffer, sizeof(buffer), 0);
                        if (bytes_received <= 0) {
                            close(i);
                            FD_CLR(i, &master_fds);
                            break;
                        }

                        uint64_t n = strtoull(buffer, NULL, 10);
                        uint64_t result = fact(n);
                        send(i, &result, sizeof(result), 0);
                    }
                }
            }
        }
    }

    return 0;
}
