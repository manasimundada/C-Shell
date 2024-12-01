#include "iMan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define HOST "man.he.net"
#define PORT 80

// Function to parse the command and return the correct query parameter
const char* parse_command(const char* command) {
    return command; // Currently no additional parsing needed
}

// Function to encode URL parameters (if needed)
void url_encode(const char* command, char* encoded_command) {
    char *ptr = encoded_command;
    for (int i = 0; command[i] != '\0'; ++i) {
        if (isalnum((unsigned char)command[i])) {
            *ptr++ = command[i];
        } else {
            sprintf(ptr, "%%%02X", (unsigned char)command[i]);
            ptr += 3;
        }
    }
    *ptr = '\0';
}

// Function to remove HTML tags from the response (basic implementation)
void strip_html_tags(char* buffer) {
    int in_tag = 0;
    char* dst = buffer;
    for (char* src = buffer; *src; src++) {
        if (*src == '<') {
            in_tag = 1;
        } else if (*src == '>') {
            in_tag = 0;
            continue;
        }
        if (!in_tag) {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

// Function to send HTTP request and receive response
void send_request(const char* command, char* response_buffer) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request_buffer[BUFFER_SIZE];
    int bytes_received;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }
    
    // Get host by name
    server = gethostbyname(HOST);
    if (server == NULL) {
        fprintf(stderr, "Error: no such host\n");
        close(sockfd);
        exit(1);
    }

    // Set up server address structure
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(PORT);
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        close(sockfd);
        exit(1);
    }
    
    // Prepare GET request
    sprintf(request_buffer, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", command, HOST);
    
    // Send request
    if (send(sockfd, request_buffer, strlen(request_buffer), 0) < 0) {
        perror("Error sending request");
        close(sockfd);
        exit(1);
    }
    
    // Receive response
    response_buffer[0] = '\0';
    while ((bytes_received = recv(sockfd, request_buffer, BUFFER_SIZE - 1, 0)) > 0) {
        request_buffer[bytes_received] = '\0';
        strcat(response_buffer, request_buffer);
    }

    if (bytes_received < 0) {
        perror("Error receiving data");
    }

    close(sockfd);
}

// Main iMan function to print the man page
void print_iman(const char* command) {
    char encoded_command[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE * 10];  // Buffer to store the complete response

    // Step 1: Parse the command
    const char* parsed_command = parse_command(command);

    // Step 2: URL encode the command
    url_encode(parsed_command, encoded_command);

    // Step 3: Send request and get the response
    send_request(encoded_command, response_buffer);

    // Step 4: Strip HTML tags from the response
    strip_html_tags(response_buffer);

    // Step 5: Print the result
    printf("%s", response_buffer);
}

// This function will be called from tokenise.c
void handle_iman(char** tokens, int token_count) {
    if (token_count < 2) {
        printf("Usage: iMan <command_name>\n");
        return;
    }
    print_iman(tokens[1]);
}
