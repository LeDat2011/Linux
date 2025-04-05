#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Định nghĩa mã màu ANSI
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"

// Hàm hiển thị header
void display_header() {
	system("clear");  // Xóa màn hình
	printf(BLUE BOLD);
	printf("╔════════════════════════════════════════╗\n");
	printf("║         QUẢN LÝ TIẾN TRÌNH            ║\n");
	printf("╚════════════════════════════════════════╝\n\n");
	printf(RESET);
}

// Hàm hiển thị menu
void display_menu() {
	printf(BLUE BOLD "MENU CHỨC NĂNG:\n" RESET);
	printf("1. Tạo tiến trình mới\n");
	printf("2. Tạm dừng tiến trình\n");
	printf("3. Tiếp tục tiến trình\n");
	printf("4. Kết thúc tiến trình\n");
	printf("5. Theo dõi tiến trình\n");
	printf("6. Xem danh sách tiến trình\n");
	printf("0. Thoát\n\n");
}

// Hàm hiển thị thông báo và đợi người dùng nhấn Enter
void display_message_wait(const char *message, const char *color) {
	printf("%s%s%s\n", color, message, RESET);
	printf("Nhấn Enter để tiếp tục...");
	getchar();
	getchar(); // Cần gọi hai lần để xử lý Enter sau scanf
}

// Hàm tạo tiến trình mới
void create_process() {
	printf(YELLOW BOLD "\nTạo tiến trình mới\n" RESET);
	printf("------------------------\n");
	
	pid_t pid = fork();
	
	if (pid == 0) {
		// Tiến trình con
		printf(GREEN "✓ Tiến trình con đã được tạo.\n" RESET);
		printf("→ Tiến trình con sẽ chạy trong 60 giây.\n");
		sleep(60);
		exit(0);
	} else if (pid > 0) {
		// Tiến trình cha
		char message[100];
		sprintf(message, "✓ PID của tiến trình con: %d", pid);
		display_message_wait(message, GREEN);
	} else {
		display_message_wait("Lỗi: Không thể tạo tiến trình mới.", RED);
	}
}

// Hàm tạm dừng tiến trình (với xác nhận)
void suspend_process() {
	printf(YELLOW BOLD "\nTạm dừng tiến trình\n" RESET);
	printf("------------------------\n");
	printf("Nhập PID của tiến trình cần tạm dừng: ");
	
	pid_t pid;
	scanf("%d", &pid);
	
	printf(YELLOW "Bạn có chắc muốn tạm dừng tiến trình %d? (y/n): " RESET, pid);
	char confirm;
	scanf(" %c", &confirm);
	
	if (confirm == 'y' || confirm == 'Y') {
		if (kill(pid, SIGSTOP) == -1) {
			char message[100];
			sprintf(message, "Lỗi: Không thể tạm dừng tiến trình %d", pid);
			display_message_wait(message, RED);
		} else {
			char message[100];
			sprintf(message, "✓ Đã tạm dừng tiến trình %d", pid);
			display_message_wait(message, GREEN);
		}
	} else {
		display_message_wait("Đã hủy thao tác tạm dừng tiến trình", YELLOW);
	}
}

// Hàm tiếp tục tiến trình
void resume_process() {
	printf(YELLOW BOLD "\nTiếp tục tiến trình\n" RESET);
	printf("------------------------\n");
	printf("Nhập PID của tiến trình cần tiếp tục: ");
	
	pid_t pid;
	scanf("%d", &pid);
	
	if (kill(pid, SIGCONT) == -1) {
		char message[100];
		sprintf(message, "Lỗi: Không thể tiếp tục tiến trình %d", pid);
		display_message_wait(message, RED);
	} else {
		char message[100];
		sprintf(message, "✓ Đã tiếp tục tiến trình %d", pid);
		display_message_wait(message, GREEN);
	}
}

// Hàm kết thúc tiến trình (với xác nhận)
void terminate_process() {
	printf(YELLOW BOLD "\nKết thúc tiến trình\n" RESET);
	printf("------------------------\n");
	printf("Nhập PID của tiến trình cần kết thúc: ");
	
	pid_t pid;
	scanf("%d", &pid);
	
	printf(RED "Cảnh báo: Bạn có chắc muốn kết thúc tiến trình %d? (y/n): " RESET, pid);
	char confirm;
	scanf(" %c", &confirm);
	
	if (confirm == 'y' || confirm == 'Y') {
		if (kill(pid, SIGTERM) == -1) {
			char message[100];
			sprintf(message, "Lỗi: Không thể kết thúc tiến trình %d", pid);
			display_message_wait(message, RED);
		} else {
			char message[100];
			sprintf(message, "✓ Đã kết thúc tiến trình %d", pid);
			display_message_wait(message, GREEN);
		}
	} else {
		display_message_wait("Đã hủy thao tác kết thúc tiến trình", YELLOW);
	}
}

// Hàm theo dõi tiến trình
void monitor_process() {
	printf(YELLOW BOLD "\nTheo dõi tiến trình\n" RESET);
	printf("------------------------\n");
	printf("Nhập PID của tiến trình cần theo dõi: ");
	
	pid_t pid;
	scanf("%d", &pid);
	
	if (waitpid(pid, NULL, WNOHANG) == 0) {
		char message[100];
		sprintf(message, "→ Tiến trình %d đang chạy", pid);
		display_message_wait(message, GREEN);
	} else {
		char message[100];
		sprintf(message, "→ Tiến trình %d đã kết thúc", pid);
		display_message_wait(message, RED);
	}
}

// Hàm hiển thị danh sách tiến trình
void list_processes() {
	printf(YELLOW BOLD "\nDanh sách tiến trình đang chạy\n" RESET);
	printf("------------------------\n");
	
	// Sử dụng popen để thực thi lệnh ps và đọc kết quả
	FILE *fp;
	char line[1024];
	
	// Hiển thị header
	printf(BLUE BOLD "  PID  TTY          TIME CMD\n" RESET);
	
	// Thực thi lệnh ps
	fp = popen("ps -e", "r");
	if (fp == NULL) {
		printf(RED "Lỗi: Không thể lấy danh sách tiến trình\n" RESET);
		return;
	}
	
	// Đọc và hiển thị kết quả
	while (fgets(line, sizeof(line), fp) != NULL) {
		printf("%s", line);
	}
	
	pclose(fp);
	printf("\nNhấn Enter để tiếp tục...");
	getchar();
	getchar();
}

int main() {
	int choice;
	
	while (1) {
		display_header();
		display_menu();
		
		printf("Nhập lựa chọn của bạn: ");
		scanf("%d", &choice);
		
		switch (choice) {
			case 1: 
				create_process(); 
				break;
			case 2: 
				suspend_process();  // Có xác nhận y/n
				break;
			case 3: 
				resume_process();
				break;
			case 4: 
				terminate_process();  // Có xác nhận y/n
				break;
			case 5: 
				monitor_process();
				break;
			case 6: 
				list_processes();
				break;
			case 0:
				printf(YELLOW "Bạn có chắc muốn thoát chương trình? (y/n): " RESET);
				char confirm;
				scanf(" %c", &confirm);
				if (confirm == 'y' || confirm == 'Y') {
					printf(GREEN BOLD "\nCảm ơn bạn đã sử dụng chương trình!\n" RESET);
					exit(0);
				}
				break;
			default:
				printf(RED "Lựa chọn không hợp lệ!\n" RESET);
				sleep(2);
		}
	}
	
	return 0;
}



