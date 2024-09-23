#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../include/router.h"
#include "../include/logger.h"

#define PORT 8080
#define CHUNK_SIZE 4096

void parse_query_string(char *query_string, char params[][2][256], int *param_count) {
    char *pair = strtok(query_string, "&");
    *param_count = 0;
    while (pair != NULL && *param_count < 10) {
        char *key = strtok(pair, "=");
        char *value = strtok(NULL, "=");
        if (key && value) {
            strncpy(params[*param_count][0], key, 256);
            strncpy(params[*param_count][1], value, 256);
            (*param_count)++;
        }
        pair = strtok(NULL, "&");
    }
}

void send_response_header(int socket, long content_length, const char *content_type) {
    char header[1024];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", content_type, content_length);
    send(socket, header, strlen(header), 0);
    log_message(LOG_DEBUG, "Sent response header: Content-Type: %s, Content-Length: %ld", content_type, content_length);
}

void send_file_in_chunks(int socket, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        log_message(LOG_ERROR, "Failed to open file: %s", file_path);
        perror("Failed to open file");
        return;
    }

    char buffer[CHUNK_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }

    fclose(file);
    log_message(LOG_INFO, "Sent file in chunks: %s", file_path);
}

char *read_file(const char *file_path, long *file_size) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    char *file_content = malloc(*file_size + 1);
    if (!file_content) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(file_content, 1, *file_size, file);
    file_content[*file_size] = '\0';

    fclose(file);
    return file_content;
}

const char *get_content_type(const char *file_path) {
    if (strstr(file_path, ".html") || strstr(file_path, ".htm")) return "text/html";
    if (strstr(file_path, ".css")) return "text/css";
    if (strstr(file_path, ".php")) return "text/html";
    if (strstr(file_path, ".js")) return "application/javascript";
    if (strstr(file_path, ".jpg") || strstr(file_path, ".jpeg")) return "image/jpeg";
    if (strstr(file_path, ".png")) return "image/png";
    if (strstr(file_path, ".json")) return "application/json";
    return "text/plain";
}

void handle_request(int new_socket) {
    char buffer[1024] = {0};

    if (read(new_socket, buffer, 1024) < 0) {
        char *response_500 = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\n500 Internal Server Error";
        send(new_socket, response_500, strlen(response_500), 0);
        log_message(LOG_ERROR, "Failed to read request.");
        close(new_socket);
        return;
    }

    log_message(LOG_INFO, "Request received:\n%s", buffer);

    char *method = strtok(buffer, " ");
    char *requested_file = strtok(NULL, " ");
    
    if (!method || !requested_file) {
        char *response_400 = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\n400 Bad Request";
        send(new_socket, response_400, strlen(response_400), 0);
        log_message(LOG_WARN, "Malformed request received.");
        close(new_socket);
        return;
    }

    if (strcmp(method, "GET") != 0) {
        char *response_405 = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: 23\r\n\r\n405 Method Not Allowed";
        send(new_socket, response_405, strlen(response_405), 0);
        log_message(LOG_WARN, "Method not allowed: %s", method);
        close(new_socket);
        return;
    }

    log_message(LOG_DEBUG, "Request method: %s, Requested file: %s", method, requested_file);

    char *query_string = NULL;
    char *path = strtok(requested_file, "?");
    query_string = strtok(NULL, "?");

    char *file_path = route_request(path);

    if (file_path) {
        FILE *file = fopen(file_path, "rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);

            const char *content_type = get_content_type(file_path);
            send_response_header(new_socket, file_size, content_type);

            fclose(file);
            send_file_in_chunks(new_socket, file_path);
            log_message(LOG_INFO, "HTTP 200 response sent for %s.", path);
        } else {
            char *response_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\n404 Not Found";
            send(new_socket, response_404, strlen(response_404), 0);
            log_message(LOG_ERROR, "File not found: %s", path);
        }
    } else {
        char *response_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\n404 Not Found";
        send(new_socket, response_404, strlen(response_404), 0);
        log_message(LOG_WARN, "Route not found for path: %s", path);
    }

    close(new_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    init_routes();

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    log_message(LOG_INFO, "Server listening on port %d", PORT); 

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        handle_request(new_socket);
    }

    close(server_fd);
    return 0;
}
