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
#define AES_ENCRYPT 0

int main() {
    int fd;
    char buffer[AES_KEY_SIZE + AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    char key[AES_KEY_SIZE] = "1234567890abcdef"; // Khóa AES-128
    char input[AES_BLOCK_SIZE] = "Hello, Kernel!!!"; // Dữ liệu đầu vào
    FILE *fp;

    // Mở thiết bị
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // Chuẩn bị buffer: key + input + output
    memcpy(buffer, key, AES_KEY_SIZE);
    memcpy(buffer + AES_KEY_SIZE, input, AES_BLOCK_SIZE);

    // Gọi ioctl để mã hóa
    if (ioctl(fd, AES_ENCRYPT, buffer) < 0) {
        perror("IOCTL failed (encrypt)");
        close(fd);
        return -1;
    }

    // In dữ liệu mã hóa
    printf("Input data: %s\n", input);
    printf("Encrypted data: ");
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        printf("%02x", (unsigned char)buffer[AES_KEY_SIZE + AES_BLOCK_SIZE + i]);
    }
    printf("\n");

    // Lưu dữ liệu mã hóa vào file
    fp = fopen("encrypted.bin", "wb");
    if (!fp) {
        perror("Failed to open file for writing");
        close(fd);
        return -1;
    }
    fwrite(buffer + AES_KEY_SIZE + AES_BLOCK_SIZE, 1, AES_BLOCK_SIZE, fp);
    fclose(fp);
    printf("Encrypted data saved to encrypted.bin\n");

    close(fd);
    return 0;
}
