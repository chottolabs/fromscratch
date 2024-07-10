#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int server_fd;

void cleanup(int signum) {
    printf("\nCleaning up and exiting...\n");
    close(server_fd);
    exit(0);
}

int main() {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    int opt = 1;

    signal(SIGINT, cleanup);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);

        char *response = "Hello from server";
        send(new_socket, response, strlen(response), 0);
        printf("Response sent\n");

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
