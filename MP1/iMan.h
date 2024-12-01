#ifndef IMAN_H
#define IMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>

// Function declarations
void fetch_man_page(const char* command);  // Fetch the man page (already present)
void handle_iman(char** tokens, int token_count);  // Handle iMan (already present)
const char* parse_command(const char* command);  // Parse the command
void url_encode(const char* command, char* encoded_command);  // URL encode the command
void strip_html_tags(char* buffer);  // Strip HTML tags from the response
void send_request(const char* command, char* response_buffer);  // Send HTTP request

#endif // IMAN_H
