#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    // Step 1: Include necessary libraries
    char url[256];
    FILE *file = fopen("website.txt", "a+"); // Output file

    printf("The URL entered should be in the format: www.example.com\n");


    while (1) {

        printf("Enter a URL (or 'exit' to quit): ");
        fgets(url, 256, stdin);
        url[strcspn(url, "\n")] = 0;
        if (strcmp(url, "exit") == 0) break;

        // Extract hostname from URL
        char hostname[256];
        size_t pos = strcspn(url, "/");
        if (pos == strlen(url)) {
            strcpy(hostname, url);
        } else {
            strncpy(hostname, url, pos);
            hostname[pos] = 0;
        }

        char request[1024];
        sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", hostname); // HTTP GET request string

        // Step 2: Initialize Winsock (not necessary on macOS)

        // Step 3: Create a socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            fprintf(stderr, "Failed to create socket\n");
            return -1;
        }

        // Step 4: Establish a connection to the web server
        struct hostent *host = gethostbyname(hostname);
        if (!host) {
            fprintf(stderr, "Failed to resolve hostname\n");
            return -1;
        }
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(80);
        server_addr.sin_addr.s_addr = *((unsigned long *) host->h_addr);
        if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            fprintf(stderr, "Failed to connect to server\n");
            return -1;
        }

        // Step 5: Send an HTTP GET request
        if (send(sock, request, strlen(request), 0) < 0) {
            fprintf(stderr, "Failed to send request\n");
            return -1;
        }

        // Step 6: Receive the server's response
        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        char *response = malloc(BUFFER_SIZE);
        int bytes_received;
        int total_bytes_received = 0;
        while ((bytes_received = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
            memcpy(response + total_bytes_received, buffer, bytes_received);
            total_bytes_received += bytes_received;
        }
        if (bytes_received < 0) {
            fprintf(stderr, "Failed to receive response\n");
            return -1;
        }

        // Step 7: Process the received data
        fwrite(response, total_bytes_received, 1, file);
        fprintf(file, "\n");

        free(response);

        // Step 8: Close the socket
        close(sock);
            printf("Successfully retrieved content for %s\n", url);
}

fclose(file);

return 0;
}
