#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    FILE *file;
    long filesize;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 将IP地址转换为网络字节序
    if(inet_pton(AF_INET, "192.168.159.133", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 接收文件大小
    recv(sock, &filesize, sizeof(long), 0);

    // 创建或覆盖文件
    file = fopen("received_file.txt", "wb");
    if (!file) {
        printf("Failed to create file\n");
        return -1;
    }

    // 根据文件大小接收数据
    while (filesize > 0) {
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        fwrite(buffer, 1, bytes_received, file);
        filesize -= bytes_received;
    }
    printf("transfer complete");
    fclose(file);
    close(sock);
    return 0;
}
