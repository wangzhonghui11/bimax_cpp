#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include  <cstring>
int main() {
    int pipefd[2];
    pipe(pipefd);  // 创建管道

    if (fork() == 0) {  // 子进程
        close(pipefd[1]);  // 关闭写端
        char buf[100];
        read(pipefd[0], buf, sizeof(buf));
        std::cout << "Child received: " << buf << std::endl;
        close(pipefd[0]);
        exit(0);
    } else {  // 父进程
        close(pipefd[0]);  // 关闭读端i
        std::cout << "parent hello"<< std::endl;        
        const char* msg = "Hello from parent";
        write(pipefd[1], msg, strlen(msg) + 1);
        close(pipefd[1]);
        wait(nullptr);  // 等待子进程
    }
    return 0;
}