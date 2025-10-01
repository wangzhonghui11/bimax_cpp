// tcp_client.cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

const char* SERVER_IP = "127.0.0.1";
const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. 创建Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket创建失败" << std::endl;
        return -1;
    }

    // 2. 连接服务器
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "无效地址" << std::endl;
        close(sock);
        return -1;
    }

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "连接失败" << std::endl;
        close(sock);
        return -1;
    }

    // 3. 发送数据
    const char* message = "Hello from client";
    send(sock, message, strlen(message), 0);

    // 4. 接收响应
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(sock, buffer, BUFFER_SIZE);
    std::cout << "服务器响应: " << buffer << std::endl;

    // 5. 关闭连接
    close(sock);
    return 0;
}