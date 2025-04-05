#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>  // Cho WEXITSTATUS
#include <unistd.h>
#include <errno.h>     // Cho perror

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
    printf("║         QUẢN LÝ TẬP TIN                ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf(RESET);
}

// Hàm hiển thị menu
void display_menu() {
    printf(BLUE BOLD "MENU CHỨC NĂNG:\n" RESET);
    printf("1. Danh sách file\n");
    printf("2. Thêm file mới\n");
    printf("3. Cập nhật nội dung file\n");
    printf("4. Xóa file\n");
    printf("5. Hiển thị nội dung file\n");
    printf("6. Thay đổi thuộc tính file\n");
    printf("7. Nén file\n");
    printf("0. Thoát\n\n");
}

// Hàm để đợi người dùng nhấn Enter
void wait_for_enter() {
    printf("\nNhấn Enter để tiếp tục...");
    fflush(stdout);
    getchar();
}

// Hàm helper để đọc input an toàn
void safe_input(char *buffer, int size) {
    while (getchar() != '\n'); // Xóa buffer trước
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

// Hiển thị danh sách file
void list_files() {
    printf(YELLOW BOLD "\nDanh sách file trong thư mục hiện tại:\n" RESET);
    printf("----------------------------------------\n");
    int status = system("ls -l --color=auto");
    if (status == -1 || WEXITSTATUS(status) != 0) {
        printf(RED "Lỗi: Không thể hiển thị danh sách file\n" RESET);
        perror("System error");
    }
    wait_for_enter();
}

// Thêm file mới
void add_new_file() {
    char filename[100];
    printf(YELLOW BOLD "\nThêm file mới\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file mới: ");
    scanf("%s", filename);
    while (getchar() != '\n'); // Xóa buffer
    
    printf(YELLOW "Bạn có chắc muốn tạo file %s? (y/n): " RESET, filename);
    char confirm = getchar();
    while (getchar() != '\n'); // Xóa buffer
    
    if (confirm == 'y' || confirm == 'Y') {
        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            printf(GREEN "✓ Đã tạo file %s thành công\n" RESET, filename);
            fclose(file);
        } else {
            printf(RED "Lỗi: Không thể tạo file\n" RESET);
        }
    } else {
        printf(YELLOW "Đã hủy tạo file\n" RESET);
    }
    wait_for_enter();
}

// Cập nhật nội dung file
void update_file_content() {
    char filename[100];
    printf(YELLOW BOLD "\nCập nhật nội dung file\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file cần chỉnh sửa: ");
    scanf("%s", filename);
    while (getchar() != '\n');

    // Kiểm tra file có tồn tại không
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(RED "Lỗi: File không tồn tại!\n" RESET);
        wait_for_enter();
        return;
    }
    fclose(file);

    printf(YELLOW "Bạn có chắc muốn sửa nội dung file %s? (y/n): " RESET, filename);
    char confirm = getchar();
    while (getchar() != '\n');
    
    if (confirm == 'y' || confirm == 'Y') {
        // Hiển thị nội dung hiện tại của file
        printf(BLUE "\nNội dung hiện tại của file:\n" RESET);
        printf("----------------------------------------\n");
        char command[200];
        sprintf(command, "cat %s", filename);
        system(command);
        printf("----------------------------------------\n");

        char editor_command[200];
        sprintf(editor_command, "nano %s", filename);

        if (system(editor_command) == 0) {
            printf(GREEN "✓ Đã cập nhật nội dung file thành công\n" RESET);
        } else {
            printf(RED "Lỗi: Không thể cập nhật nội dung file\n" RESET);
        }
    } else {
        printf(YELLOW "Đã hủy cập nhật nội dung file\n" RESET);
    }
    wait_for_enter();
}

// Xóa file
void delete_file() {
    char filename[100];
    printf(YELLOW BOLD "\nXóa file\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file cần xóa: ");
    scanf("%s", filename);
    while (getchar() != '\n');
    
    printf(RED "Cảnh báo: Bạn có chắc muốn xóa file %s? (y/n): " RESET, filename);
    char confirm = getchar();
    while (getchar() != '\n');
    
    if (confirm == 'y' || confirm == 'Y') {
        if (remove(filename) == 0) {
            printf(GREEN "✓ Đã xóa file %s thành công\n" RESET, filename);
        } else {
            printf(RED "Lỗi: Không thể xóa file\n" RESET);
        }
    } else {
        printf(YELLOW "Đã hủy xóa file\n" RESET);
    }
    wait_for_enter();
}

// Hiển thị nội dung file
void display_file_content() {
    char filename[100];
    printf(YELLOW BOLD "\nHiển thị nội dung file\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file cần hiển thị: ");
    scanf("%s", filename);
    while (getchar() != '\n');
    
    // Kiểm tra file có tồn tại không
    if (access(filename, F_OK) == -1) {
        printf(RED "Lỗi: File không tồn tại!\n" RESET);
        wait_for_enter();
        return;
    }
    
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        printf(BLUE "\nNội dung file %s:\n" RESET, filename);
        printf("----------------------------------------\n");
        
        // Đọc và hiển thị nội dung file
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }
        
        printf("\n----------------------------------------\n");
        fclose(file);
    } else {
        printf(RED "Lỗi: Không thể mở file %s\n" RESET, filename);
    }
    wait_for_enter();
}

// Thay đổi thuộc tính file
void change_file_attribute() {
    char filename[256];
    int attribute_choice;
    
    printf(YELLOW BOLD "\nThay đổi thuộc tính file\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file cần thay đổi thuộc tính: ");
    scanf("%s", filename);
    while (getchar() != '\n');

    if (access(filename, F_OK) == -1) {
        printf(RED "Lỗi: File không tồn tại!\n" RESET);
        wait_for_enter();
        return;
    }

    printf("\nChọn loại thuộc tính muốn thay đổi:\n");
    printf("1. Chỉ đọc (read-only)\n");
    printf("2. Đọc và ghi (read-write)\n");
    printf("3. File ẩn\n");
    printf("0. Quay lại menu chính\n");
    printf("Lựa chọn của bạn: ");
    scanf("%d", &attribute_choice);
    while (getchar() != '\n');

    switch (attribute_choice) {
        case 1:
            if (chmod(filename, S_IRUSR | S_IRGRP | S_IROTH) == 0) {
                printf(GREEN "✓ Đã thay đổi %s thành chỉ đọc\n" RESET, filename);
            } else {
                perror(RED "Lỗi" RESET);
            }
            wait_for_enter();
            break;
        case 2:
            if (chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IXUSR) == 0) {
                printf(GREEN "✓ Đã thay đổi %s thành đọc và ghi\n" RESET, filename);
            } else {
                perror(RED "Lỗi" RESET);
            }
            wait_for_enter();
            break;
        case 3:
            char newname[257];
            snprintf(newname, sizeof(newname), ".%s", filename);
            if (rename(filename, newname) == 0) {
                printf(GREEN "✓ Đã chuyển %s thành file ẩn\n" RESET, filename);
            } else {
                printf(RED "Lỗi: Không thể chuyển thành file ẩn\n" RESET);
            }
            wait_for_enter();
            break;
        case 0:
            return;
        default:
            printf(RED "Lựa chọn không hợp lệ!\n" RESET);
            wait_for_enter();
    }
}

// Nén file
void compress_file() {
    char input_name[256];
    char output_name[256];

    printf(YELLOW BOLD "\nNén file\n" RESET);
    printf("----------------------------------------\n");
    printf("Nhập tên file cần nén: ");
    scanf("%s", input_name);
    while (getchar() != '\n');

    if (access(input_name, F_OK) == -1) {
        printf(RED "Lỗi: File không tồn tại!\n" RESET);
        wait_for_enter();
        return;
    }

    printf("Nhập tên cho file nén (không cần thêm .tar.gz): ");
    scanf("%s", output_name);
    while (getchar() != '\n');

    char tar_filename[300];
    snprintf(tar_filename, sizeof(tar_filename), "%s.tar.gz", output_name);
    
    if (access(tar_filename, F_OK) != -1) {
        printf(RED "Lỗi: File nén %s đã tồn tại!\n" RESET, tar_filename);
        wait_for_enter();
        return;
    }

    printf(YELLOW "Bạn có chắc muốn nén file %s? (y/n): " RESET, input_name);
    char confirm = getchar();
    while (getchar() != '\n');
    
    if (confirm == 'y' || confirm == 'Y') {
        char command[600];
        snprintf(command, sizeof(command), "tar -czf %s %s", tar_filename, input_name);
        
        if (system(command) == 0) {
            printf(GREEN "✓ Đã nén %s thành %s thành công\n" RESET, 
                   input_name, tar_filename);
        } else {
            printf(RED "Lỗi: Không thể nén file\n" RESET);
        }
    } else {
        printf(YELLOW "Đã hủy nén file\n" RESET);
    }
    wait_for_enter();
}

int main() {
    int choice;
    
    while (1) {
        display_header();
        display_menu();
        
        printf("Nhập lựa chọn của bạn: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Xóa buffer nếu input không hợp lệ
            printf(RED "Lỗi: Vui lòng nhập số\n" RESET);
            wait_for_enter();
            continue;
        }
        while (getchar() != '\n'); // Xóa buffer sau khi đọc số
        
        switch (choice) {
            case 1: list_files(); break;
            case 2: add_new_file(); break;
            case 3: update_file_content(); break;
            case 4: delete_file(); break;
            case 5: display_file_content(); break;
            case 6: change_file_attribute(); break;
            case 7: compress_file(); break;
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

