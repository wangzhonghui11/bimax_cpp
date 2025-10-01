// main.cpp
#include "circular_buffer.h"
#include <iostream>
#include <thread>
#include <random>
#include <chrono>

void producer(CircularBuffer& buffer, std::atomic<bool>& running) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);

    while (running) {
        std::string data = "Value: " + std::to_string(dis(gen));
        if (!buffer.push(
            reinterpret_cast<const u8*>(data.c_str()),
            data.size() + 1 // +1 for null terminator
        )) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void consumer(CircularBuffer& buffer, std::atomic<bool>& running) {
    u8 temp_buf[256];
    while (running) {
        size_t len = buffer.pop(temp_buf, sizeof(temp_buf));
        if (len > 0) {
            std::cout << reinterpret_cast<char*>(temp_buf) << std::endl;
        }
    }
}

int main() {
    CircularBuffer buffer(1024);
    std::atomic<bool> running{true};

    std::thread prod_thread(producer, std::ref(buffer), std::ref(running));
    std::thread cons_thread(consumer, std::ref(buffer), std::ref(running));

    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();
    running = false;
    buffer.stop();

    prod_thread.join();
    cons_thread.join();
    return 0;
}