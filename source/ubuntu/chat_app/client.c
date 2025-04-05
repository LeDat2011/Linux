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

#define LENGTH 2048
#define NAME_LEN 32
#define MAX_FILE_SIZE 1024*1024  // Giới hạn file 1MB
#define FILE_CHUNK_SIZE 8192     // Kích thước mỗi chunk gửi
#define FILE_CMD "/file"         // Lệnh gửi file

// Định nghĩa màu sắc
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];

void print_welcome_banner() {
  printf("\n%s%s╔════════════════════════════════════════╗%s\n", BOLD, BLUE, RESET);
    printf("%s%s║          WELCOME TO CHAT ROOM          ║%s\n", BOLD, BLUE, RESET);
    printf("%s%s╚════════════════════════════════════════╝%s\n\n", BOLD, BLUE, RESET);
}

void print_help_menu() {
    printf("\n%s%s=== Available Commands ===%s\n", BOLD, CYAN, RESET);
    printf("%s/help%s    - Show this help menu\n", GREEN, RESET);
    printf("%s/users%s   - Show online users\n", GREEN, RESET);
    printf("%s/clear%s   - Clear screen\n", GREEN, RESET);
    printf("%s/quit%s    - Exit chat room\n", GREEN, RESET);
    printf("%s/name%s    - Show your name\n", GREEN, RESET);
    printf("%s/file%s    - Send file (/file <filename>)\n", GREEN, RESET);
    printf("\n%s%s=== Chat Tips ===%s\n", BOLD, CYAN, RESET);
    printf("- Type your message and press Enter to send\n");
    printf("- Use /file command to send files\n");
    printf("- Press Ctrl+C to exit\n");
    printf("%s%s======================%s\n\n", BOLD, CYAN, RESET);
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void print_prompt() {
    printf("%s%s[%s%s%s%s]%s > ", BOLD, BLUE, RESET, name, BOLD, BLUE, RESET);
    fflush(stdout);
}

void str_trim_lf(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit() {
    flag = 1;
}

void handle_command(char* message) {
    if (strcmp(message, "/help") == 0) {
        print_help_menu();
    }
    else if (strcmp(message, "/clear") == 0) {
        clear_screen();
        print_welcome_banner();
    }
    else if (strcmp(message, "/name") == 0) {
        printf("%sYour name is: %s%s\n", YELLOW, name, RESET);
    }
    else if (strncmp(message, "/file ", 6) == 0) {
        char* filename = message + 6;
        if (strlen(filename) > 0) {
            send_file(filename);
        } else {
            printf("%sUsage: /file <filename>%s\n", RED, RESET);
        }
    }
    else if (strcmp(message, "/quit") == 0 || 
             strcmp(message, "quit") == 0 || 
             strcmp(message, "exit") == 0) {
        printf("%s%sBye bye!%s\n", BOLD, YELLOW, RESET);
        flag = 1;
    }
}

void recv_msg_handler() {
    char message[LENGTH] = {};
    char filename[256] = {};
    long file_size = 0;
    FILE* file = NULL;

    while (1) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            if (strncmp(message, "FILE_START:", 11) == 0) {
                // Parse thông tin file
                char* file_info = message + 11;
                char* size_str = strchr(file_info, ':');
                if (size_str) {
                    *size_str = '\0';
                    size_str++;
                    strcpy(filename, file_info);
                    file_size = atol(size_str);

                    // Tạo tên file mới để nhận
                    char new_filename[300];
                    sprintf(new_filename, "received_%s", filename);
                    file = fopen(new_filename, "wb");
                    
                    if (!file) {
                        printf("%sError: Cannot create file for receiving%s\n", RED, RESET);
                        continue;
                    }
                    printf("%sReceiving file: %s (Size: %ld bytes)%s\n", 
                           YELLOW, new_filename, file_size, RESET);
                }
            }
            else if (strncmp(message, "FILE_END:", 9) == 0) {
                if (file) {
                    fclose(file);
                    file = NULL;
                    printf("%sFile received successfully!%s\n", GREEN, RESET);
                }
            }
            else if (file) {
                // Đang nhận file
                fwrite(message, 1, receive, file);
            }
            else {
                // Tin nhắn thông thường
                printf("\r%s", message);
                print_prompt();
            }
        } else if (receive == 0) {
            break;
        }
        memset(message, 0, sizeof(message));
    }
}

void send_msg_handler() {
    char message[LENGTH] = {};
    char buffer[LENGTH + NAME_LEN] = {};

    print_help_menu();

    while (1) {
        print_prompt();
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        if (message[0] == '/') {
            handle_command(message);
            if (flag) break;
        } else if (strlen(message) > 0) {
            sprintf(buffer, "%s%s%s: %s%s\n", BOLD, CYAN, name, message, RESET);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + NAME_LEN);
    }
    catch_ctrl_c_and_exit();
}

void send_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("%sError: Cannot open file %s%s\n", RED, filename, RESET);
        return;
    }

    // Lấy kích thước file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > MAX_FILE_SIZE) {
        printf("%sError: File size too large (max: 1MB)%s\n", RED, RESET);
        fclose(file);
        return;
    }

    // Gửi header cho file
    char header[LENGTH];
    sprintf(header, "FILE_START:%s:%ld", filename, file_size);
    send(sockfd, header, strlen(header), 0);
    
    // Đợi xác nhận từ server
    sleep(1);

    // Gửi nội dung file
    char buffer[FILE_CHUNK_SIZE];
    size_t bytes_read;
    long total_sent = 0;

    printf("%sSending file: %s%s\n", YELLOW, filename, RESET);
    printf("Progress: ");

    while ((bytes_read = fread(buffer, 1, FILE_CHUNK_SIZE, file)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) < 0) {
            printf("\n%sError sending file%s\n", RED, RESET);
            break;
        }
        total_sent += bytes_read;
        
        // Hiển thị tiến độ
        float progress = (float)total_sent / file_size * 100;
        printf("\r%sProgress: %.1f%%%s", YELLOW, progress, RESET);
        fflush(stdout);
    }

    // Gửi kết thúc file
    sprintf(header, "FILE_END:%s", filename);
    send(sockfd, header, strlen(header), 0);

    printf("\n%sFile sent successfully!%s\n", GREEN, RESET);
    fclose(file);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("%sUsage: %s <port>%s\n", RED, argv[0], RESET);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    signal(SIGINT, catch_ctrl_c_and_exit);

    clear_screen();
    print_welcome_banner();

    printf("%sPlease enter your name: %s", YELLOW, RESET);
    fgets(name, NAME_LEN, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > NAME_LEN - 1 || strlen(name) < 2) {
        printf("%sName must be between 2 and %d characters.%s\n", 
               RED, NAME_LEN - 1, RESET);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Connecting to the server
    printf("%sConnecting to server...%s\n", YELLOW, RESET);
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("%sERROR: Could not connect to server%s\n", RED, RESET);
        return EXIT_FAILURE;
    }

    // Send name
    send(sockfd, name, NAME_LEN, 0);
    printf("%s%sSuccessfully connected to chat room!%s\n", BOLD, GREEN, RESET);

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
        printf("%sERROR: pthread%s\n", RED, RESET);
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
        printf("%sERROR: pthread%s\n", RED, RESET);
        return EXIT_FAILURE;
    }

    while (1) {
        if (flag) {
            break;
        }
        sleep(1);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

