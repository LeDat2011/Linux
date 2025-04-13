#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_NAME "/dev/aes_dev"
#define AES_KEY_SIZE 16
#define AES_BLOCK_SIZE 16

// Định nghĩa lệnh ioctl
#define AES_DECRYPT 1

int main() {
    int fd;
    char buffer[AES_KEY_SIZE + AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    char key[AES_KEY_SIZE] = "1234567890abcdef"; // Khóa AES-128 (phải giống với encrypt.c)
    char encrypted[AES_BLOCK_SIZE];
    FILE *fp;

    // Đọc dữ liệu mã hóa từ file
    fp = fopen("encrypted.bin", "rb");
    if (!fp) {
        perror("Failed to open file for reading");
        return -1;
    }
    if (fread(encrypted, 1, AES_BLOCK_SIZE, fp) != AES_BLOCK_SIZE) {
        perror("Failed to read encrypted data");
        fclose(fp);
        return -1;
    }
    fclose(fp);

    // Mở thiết bị
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // Chuẩn bị buffer: key + encrypted + output
    memcpy(buffer, key, AES_KEY_SIZE);
    memcpy(buffer + AES_KEY_SIZE, encrypted, AES_BLOCK_SIZE);

    // Gọi ioctl để giải mã
    if (ioctl(fd, AES_DECRYPT, buffer) < 0) {
        perror("IOCTL failed (decrypt)");
        close(fd);
        return -1;
    }

    // In dữ liệu giải mã
    printf("Decrypted data: %.*s\n", AES_BLOCK_SIZE, buffer + AES_KEY_SIZE + AES_BLOCK_SIZE);

    close(fd);
    return 0;
}
