// circular_buffer.cpp
#include "circular_buffer.h"

CircularBuffer::CircularBuffer(size_t size) 
    : m_buf(new u8[size]), m_capacity(size) {}

CircularBuffer::~CircularBuffer() {
    delete[] m_buf;
}

bool CircularBuffer::push(const u8* data, size_t len) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, [this, len] { 
        return space_available() >= len || m_stop; 
    });
    if (m_stop) return false;

    size_t first_chunk = std::min(len, m_capacity - m_tail);
    memcpy(m_buf + m_tail, data, first_chunk);
    if (len > first_chunk) {
        memcpy(m_buf, data + first_chunk, len - first_chunk);
    }
    m_tail = (m_tail + len) % m_capacity;
    m_cond.notify_one();
    return true;
}

size_t CircularBuffer::pop(u8* output, size_t max_len) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait(lock, [this] { 
        return data_available() > 0 || m_stop; 
    });
    if (m_stop) return 0;

    size_t to_read = std::min(max_len, data_available());
    size_t first_chunk = std::min(to_read, m_capacity - m_head);
    memcpy(output, m_buf + m_head, first_chunk);
    if (to_read > first_chunk) {
        memcpy(output + first_chunk, m_buf, to_read - first_chunk);
    }
    m_head = (m_head + to_read) % m_capacity;
    m_cond.notify_one();
    return to_read;
}

void CircularBuffer::stop() {
    m_stop = true;
    m_cond.notify_all();
}

size_t CircularBuffer::space_available() const {
    return (m_head > m_tail) ? (m_head - m_tail - 1) 
           : (m_capacity - m_tail + m_head - 1);
}

size_t CircularBuffer::data_available() const {
    return (m_tail >= m_head) ? (m_tail - m_head)
           : (m_capacity - m_head + m_tail);
}