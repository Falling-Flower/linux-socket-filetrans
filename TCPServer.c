#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    FILE *file;
    char *filename = "instruct.sh"; // 要发送的文件名

    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字到指定地址和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听传入连接
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 接受新的连接
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    file = fopen(filename, "rb");  // 以二进制模式打开文件
    if (!file) {
        perror("File open failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 发送文件大小给客户端
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);
    send(new_socket, &filesize, sizeof(long), 0);

    // 读取文件并发送数据
    while (1) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        if (bytes_read > 0) {
            send(new_socket, buffer, bytes_read, 0);
        } else {
            printf("transfer complete");
            break;  // 文件结束
        }
    }

    fclose(file);
    close(new_socket);
    close(server_fd);
    return 0;
}
