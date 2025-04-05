#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

// Định nghĩa các hằng số
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048
#define NAME_LEN 32
#define ROOM_LEN 32

// Định nghĩa message types
#define MSG_CHAT    0
#define MSG_FILE    1
#define MSG_JOIN    2
#define MSG_LIST    3
#define MSG_EXIT    4
#define MAX_MSG_LEN 2048

// Định nghĩa màu cho output
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

struct message_header {
    uint8_t type;
    uint32_t length;
    char room[ROOM_LEN];
};

/* Client structure */
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LEN];
    char room[ROOM_LEN];
    int is_receiving_file;  // Trạng thái nhận file
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout() {
    printf("\r%s", "> ");
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

void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add client to queue */
void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Remove client from queue */
void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients in same room */
void send_message(char *s, int uid, char *room) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->uid != uid && strcmp(clients[i]->room, room) == 0) {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0) {
                    printf("ERROR: write to descriptor failed\n");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message_with_type(int type, char *content, int sender_uid) {
    if (!content) return;  // Kiểm tra null pointer
    
    pthread_mutex_lock(&clients_mutex);
    
    struct message_header header;
    header.type = type;
    header.length = strlen(content);

    for(int i=0; i<MAX_CLIENTS; ++i) {
        if(clients[i]) {
            // Sửa lỗi khi sender_uid = -1
            if(sender_uid == -1 || 
               (clients[i]->uid != sender_uid && 
                strcmp(clients[i]->room, clients[sender_uid]->room) == 0)) {
                send(clients[i]->sockfd, &header, sizeof(header), 0);
                send(clients[i]->sockfd, content, header.length, 0);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg) {
    char buff_out[BUFFER_SIZE];
    char name[32];
    int leave_flag = 0;

    cli_count++;
    client_t *cli = (client_t *)arg;
    cli->is_receiving_file = 0;

    // Name
    if (recv(cli->sockfd, name, 32, 0) <= 0) {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    } else {
        strcpy(cli->name, name);
        sprintf(buff_out, "%s has joined\n", cli->name);
        printf("%s", buff_out);
        strcpy(cli->room, "general");
        send_message(buff_out, cli->uid, cli->room);
    }

    bzero(buff_out, BUFFER_SIZE);

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->sockfd, buff_out, BUFFER_SIZE, 0);
        if (receive > 0) {
            if (strncmp(buff_out, "FILE_START:", 11) == 0) {
                // Chuyển tiếp thông tin file cho các client khác
                cli->is_receiving_file = 1;
                send_message(buff_out, cli->uid, cli->room);
            }
            else if (strncmp(buff_out, "FILE_END:", 9) == 0) {
                // Kết thúc nhận file
                cli->is_receiving_file = 0;
                send_message(buff_out, cli->uid, cli->room);
            }
            else if (cli->is_receiving_file) {
                // Đang nhận file, chuyển tiếp data
                send_message(buff_out, cli->uid, cli->room);
            }
            else if (strlen(buff_out) > 0) {
                // Tin nhắn thông thường
                send_message(buff_out, cli->uid, cli->room);
                str_trim_lf(buff_out, strlen(buff_out));
                printf("%s -> %s\n", buff_out, cli->name);
            }
        } else if (receive == 0 || strcmp(buff_out, "exit") == 0) {
            sprintf(buff_out, "%s has left\n", cli->name);
            printf("%s", buff_out);
            send_message(buff_out, cli->uid, cli->room);
            leave_flag = 1;
        } else {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buff_out, BUFFER_SIZE);
    }

    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

    return NULL;
}

void handle_file_transfer(client_t *cli, char *filename) {
	char notice[BUFFER_SIZE];
	sprintf(notice, "SERVER: %s is sending file: %s\n", cli->name, filename);
	send_message_with_type(MSG_CHAT, notice, -1);
	
	long file_size;
	recv(cli->sockfd, &file_size, sizeof(file_size), 0);
	
	char buffer[BUFFER_SIZE];
	while (file_size > 0) {
		int bytes = recv(cli->sockfd, buffer, 
						(file_size < BUFFER_SIZE) ? file_size : BUFFER_SIZE, 0);
		if (bytes <= 0) break;
		
		send_message_with_type(MSG_FILE, buffer, cli->uid);
		file_size -= bytes;
	}
}

void send_user_list(client_t *cli) {
	char list[BUFFER_SIZE] = "=== Users in room ===\n";
	pthread_mutex_lock(&clients_mutex);
	
	for(int i=0; i<MAX_CLIENTS; ++i) {
		if(clients[i] && strcmp(clients[i]->room, cli->room) == 0) {
			strcat(list, "- ");
			strcat(list, clients[i]->name);
			strcat(list, "\n");
		}
	}
	strcat(list, "==================\n");
	
	pthread_mutex_unlock(&clients_mutex);
	
	// Gửi trực tiếp cho client yêu cầu
	struct message_header header;
	header.type = MSG_CHAT;
	header.length = strlen(list);
	send(cli->sockfd, &header, sizeof(header), 0);
	send(cli->sockfd, list, header.length, 0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);
    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t tid;

    /* Socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);

    /* Ignore pipe signals */
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option)) < 0) {
        perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
    }

    /* Bind */
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    /* Listen */
    if (listen(listenfd, 10) < 0) {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

    printf("=== CHAT ROOM SERVER STARTED ===\n");

    while (1) {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

        /* Check if max clients is reached */
        if ((cli_count + 1) == MAX_CLIENTS) {
            printf("Max clients reached. Rejected: ");
            close(connfd);
            continue;
        }

        /* Client settings */
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = connfd;
        cli->uid = uid++;

        /* Add client to queue and fork thread */
        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void*)cli);

        /* Reduce CPU usage */
        sleep(1);
    }

    return EXIT_SUCCESS;
}

