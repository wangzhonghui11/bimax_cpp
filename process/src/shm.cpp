#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include  <cstring>
int main() {
    // 创建共享内存
    int shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666);
    char* shm_ptr = (char*)shmat(shmid, nullptr, 0);

    if (fork() == 0) {  // 子进程
        strcpy(shm_ptr, "Shared memory data");
        shmdt(shm_ptr);
    } else {  // 父进程
        wait(nullptr);
        std::cout << "Parent read: " << shm_ptr << std::endl;
        shmdt(shm_ptr);
        shmctl(shmid, IPC_RMID, nullptr);  // 删除共享内存
    }
}