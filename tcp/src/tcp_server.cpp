// tcp_server.cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    // 1. 创建Socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket创建失败" << std::endl;
        return -1;
    }

    // 2. 绑定地址
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "绑定失败" << std::endl;
        close(server_fd);
        return -1;
    }

    // 3. 监听连接
    if (listen(server_fd, 5) < 0) {
        std::cerr << "监听失败" << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "服务端启动，监听端口 " << PORT << std::endl;

    // 4. 接受连接
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "接受连接失败" << std::endl;
        close(server_fd);
        return -1;
    }

    // 5. 数据收发
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    std::cout << "收到消息: " << buffer << std::endl;

    const char* response = "Hello from server";
    send(client_fd, response, strlen(response), 0);

    // 6. 关闭连接
    close(client_fd);
    close(server_fd);
    return 0;
}