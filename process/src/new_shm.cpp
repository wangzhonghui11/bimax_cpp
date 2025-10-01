#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/wait.h>

const char* SHM_NAME = "/myshm";
const size_t SHM_SIZE = 1024;

int main() {
    // 1. 创建或打开共享内存对象
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    // 2. 调整共享内存大小
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        close(fd);
        return 1;
    }

    // 3. 内存映射
    void* ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        return 1;
    }
    close(fd);  // 映射后可直接关闭文件描述符

    // 4. 进程间通信演示
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        munmap(ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
        return 1;
    }

    if (pid == 0) {  // 子进程（消费者）
        std::cout << "[Child] Waiting for data..." << std::endl;
        
        // 等待数据就绪（简单同步）
        while (static_cast<char*>(ptr)[0] == 0) {
            usleep(100000);  // 100ms检查一次
        }

        std::cout << "[Child] Received: " << static_cast<char*>(ptr) << std::endl;
        
        // 发送响应
        const char* response = "Message received by child";
        memcpy(ptr, response, strlen(response) + 1);
    } 
    else {  // 父进程（生产者）
        const char* message = "Hello from parent process";
        std::cout << "[Parent] Sending: " << message << std::endl;
        
        // 写入数据
        memcpy(ptr, message, strlen(message) + 1);
        
        // 等待子进程响应
        sleep(1);  // 简单延迟，实际应用应使用信号量
        
        std::cout << "[Parent] Response: " << static_cast<char*>(ptr) << std::endl;
        
        wait(nullptr);  // 等待子进程结束
    }

    // 5. 清理资源
    munmap(ptr, SHM_SIZE);
    if (pid != 0) {  // 只有父进程执行unlink
        shm_unlink(SHM_NAME);
        std::cout << "[Parent] Shared memory released" << std::endl;
    }
    return 0;
}