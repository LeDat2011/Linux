#ifndef CHAT_H
#define CHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>  // Thêm header cho va_start, va_end

// Constants
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048
#define NAME_LEN 32
#define ROOM_LEN 32

// Colors for terminal output
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// Message types
#define MSG_CHAT    0
#define MSG_JOIN    1
#define MSG_LEAVE   2
#define MSG_LIST    3
#define MSG_ERROR   4

// Client structure
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int id;
    char name[NAME_LEN];
    char room[ROOM_LEN];
} client_t;

// Message structure
typedef struct {
    uint8_t type;
    char sender[NAME_LEN];
    char content[BUFFER_SIZE];
    char room[ROOM_LEN];
} message_t;

// Function declarations
void str_trim_lf(char* arr, int length);
void print_message(const char* format, ...);
void error_exit(const char* message);
int create_socket(void);
void set_socket_options(int sockfd);

#endif // CHAT_H
