#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Định nghĩa mã màu ANSI
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"

// Hàm hiển thị header
void display_header() {
    system("clear");
    printf(BLUE BOLD);
    printf("╔════════════════════════════════════════╗\n");
    printf("║         QUẢN LÝ MẠNG                   ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf(RESET);
}

// Hàm hiển thị menu
void show_menu() {
    printf(BLUE BOLD "MENU CHỨC NĂNG:\n" RESET);
    printf("1. Kiểm tra trạng thái kết nối mạng\n");
    printf("2. Kết nối wifi\n");
    printf("3. Cài đặt và sử dụng ứng dụng quản lý mạng\n");
    printf("0. Thoát\n\n");
}

// Hàm đợi người dùng nhấn Enter
void wait_for_enter() {
    printf("\nNhấn Enter để tiếp tục...");
    fflush(stdout);
    getchar();
}

// Kiểm tra kết nối mạng
void check_network() {
    printf(YELLOW BOLD "\nKiểm tra kết nối mạng\n" RESET);
    printf("----------------------------------------\n");
    
    printf("Đang kiểm tra kết nối...\n");
    int ret = system("ping -c 1 google.com > /dev/null 2>&1");
    
    if (ret == 0) {
        printf(GREEN "✓ Máy tính đang kết nối đến mạng\n" RESET);
    } else {
        printf(RED "✕ Máy tính không kết nối đến mạng\n" RESET);
    }
    wait_for_enter();
}

// Kết nối Wifi
void connect_wifi() {
    char ssid[256];
    char password[256];
    
    printf(YELLOW BOLD "\nKết nối Wifi\n" RESET);
    printf("----------------------------------------\n");
    
    printf("Nhập tên mạng (SSID): ");
    fgets(ssid, sizeof(ssid), stdin);
    ssid[strcspn(ssid, "\n")] = 0;
    
    printf("Nhập mật khẩu: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    
    printf("\nĐang kết nối đến mạng %s...\n", ssid);
    
    char command[1024];
    snprintf(command, sizeof(command), "nmcli dev wifi connect '%s' password '%s'", ssid, password);
    int ret = system(command);
    
    if (ret == 0) {
        printf(GREEN "✓ Kết nối wifi thành công\n" RESET);
    } else {
        printf(RED "✕ Kết nối wifi thất bại\n" RESET);
    }
    wait_for_enter();
}

// Cài đặt và sử dụng ứng dụng quản lý mạng
void manage_network() {
    printf(YELLOW BOLD "\nCài đặt ứng dụng quản lý mạng\n" RESET);
    printf("----------------------------------------\n");
    
    printf("Đang cài đặt Network Manager...\n");
    int ret = system("sudo apt-get install network-manager");
    
    if (ret == 0) {
        printf(GREEN "✓ Đã cài đặt Network Manager thành công\n" RESET);
        printf("\nĐang khởi động Network Manager...\n");
        system("sudo systemctl start NetworkManager");
        printf(GREEN "✓ Đã khởi động Network Manager\n" RESET);
    } else {
        printf(RED "✕ Cài đặt Network Manager thất bại\n" RESET);
    }
    wait_for_enter();
}

int main() {
    int choice;
    
    while (1) {
        display_header();
        show_menu();
        
        printf("Nhập lựa chọn của bạn: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf(RED "Lỗi: Vui lòng nhập số\n" RESET);
            wait_for_enter();
            continue;
        }
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: 
                check_network(); 
                break;
            case 2: 
                connect_wifi(); 
                break;
            case 3: 
                manage_network(); 
                break;
            case 0:
                printf(GREEN BOLD "\nCảm ơn bạn đã sử dụng chương trình!\n" RESET);
                exit(0);
            default:
                printf(RED "Lựa chọn không hợp lệ!\n" RESET);
                wait_for_enter();
        }
    }
    
    return 0;
}

