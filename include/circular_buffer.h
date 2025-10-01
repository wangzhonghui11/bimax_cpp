// circular_buffer.h
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

// 类型定义
using u8 = uint8_t;

class CircularBuffer {
public:
    explicit CircularBuffer(size_t size);
    ~CircularBuffer();

    // 禁用拷贝和移动
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;

    bool push(const u8* data, size_t len);
    size_t pop(u8* output, size_t max_len);
    void stop();

private:
    size_t space_available() const;
    size_t data_available() const;

    u8* const m_buf;
    const size_t m_capacity;
    size_t m_head = 0;
    size_t m_tail = 0;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_stop{false};
};

#endif // CIRCULAR_BUFFER_H