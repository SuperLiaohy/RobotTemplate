//
// Created by liaohy on 8/31/25.
//

#pragma once

#include <cstdint>
#include <atomic>
#include <cstring>
#include <vector>
#include <concepts>

namespace EP::Component {
template<typename T>
concept isContainer = requires(T t) {
    {t.size()} -> std::convertible_to<std::size_t>;
};

namespace RingBufferHandle {
    class Normal {
    public:
        Normal() : write_handle(0), read_handle(0) {};
        std::size_t write_handle;
        std::size_t read_handle;
    };
    class Atomic {
    public:
        Atomic() : write_handle(0), read_handle(0) {};
        std::atomic<std::size_t> write_handle;
        std::atomic<std::size_t> read_handle;
    };

    template<typename T>
    concept isHandle = requires(T t) {
        t.write_handle;
        t.read_handle;
    };
}

template<std::size_t N, typename T = uint8_t, RingBufferHandle::isHandle Handle = RingBufferHandle::Normal>
class RingBuffer : public Handle {
public:
    explicit RingBuffer() : Handle() {}

    const T* getContainer() const {return buffer;}

    [[nodiscard]] bool isEmpty() const { return Handle::write_handle == Handle::read_handle; }
    [[nodiscard]] bool isFull() const {
        if constexpr (is2Power())
            return ((Handle::write_handle + 1) & mask()) == Handle::read_handle;
        else
            return ((Handle::write_handle + 1) % capacity()) == Handle::read_handle;
    }

    [[nodiscard]] consteval static std::size_t mask() { return capacity() - 1; }

    [[nodiscard]] consteval static std::size_t capacity() { return N; }

    [[nodiscard]] std::size_t size() const {
        if constexpr (is2Power())
            return (capacity() + Handle::write_handle - Handle::read_handle) & mask();
        else
            return (capacity() + Handle::write_handle - Handle::read_handle) % capacity();
    }

    [[nodiscard]] std::size_t available() const {return mask() - size();}

    [[nodiscard]] std::size_t getReadIndex() const { return Handle::read_handle; }
    [[nodiscard]] std::size_t getWriteIndex() const { return Handle::write_handle; }

    T operator[](const std::size_t index) const {
        if constexpr (is2Power())
            return buffer[(Handle::read_handle + index) & mask()];
        else
            return buffer[(Handle::read_handle + index) % capacity()];
    }

    T head() const { return buffer[Handle::read_handle]; }

    bool write(const T* data, std::size_t len) {
        if (available() < len)
            return false;
        if constexpr (is2Power()) {
            const std::size_t first_chunk = std::min(capacity() - (Handle::write_handle & mask()), len);
            std::memcpy(&buffer[Handle::write_handle & mask()], data, first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(&buffer[0], data + first_chunk, (len - first_chunk) * sizeof(T));
            Handle::write_handle = (Handle::write_handle + len) & mask();
        } else {
            const std::size_t first_chunk = std::min(capacity() - Handle::write_handle, len);
            std::memcpy(&buffer[Handle::write_handle], data, first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(&buffer[0], data + first_chunk, (len - first_chunk) * sizeof(T));
            Handle::write_handle = (Handle::write_handle + len) % capacity();
        }
        return true;
    }

    bool writeForce(const T* data, std::size_t len) {
        if constexpr (is2Power()) {
            if (available() < len)
                Handle::read_handle = (Handle::read_handle + len - available()) & mask();
            const std::size_t first_chunk = std::min(capacity() - (Handle::write_handle & mask()), len);
            std::memcpy(&buffer[Handle::write_handle & mask()], data, first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(&buffer[0], data + first_chunk, (len - first_chunk) * sizeof(T));
            Handle::write_handle = (Handle::write_handle + len) & mask();
        }else{
            if (available() < len)
                Handle::read_handle = (Handle::read_handle + len - available()) % capacity();
            const std::size_t first_chunk = std::min(capacity() - Handle::write_handle, len);
            std::memcpy(&buffer[Handle::write_handle & mask()], data, first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(&buffer[0], data + first_chunk, (len - first_chunk) * sizeof(T));
            Handle::write_handle = (Handle::write_handle + len) % capacity();
        }
        return true;
    }

    bool read(T* data, std::size_t len) {
        if constexpr(is2Power()) {
            if (size() < len)
                return false;
            const std::size_t first_chunk = std::min(capacity() - (Handle::read_handle & mask()), len);
            std::memcpy(data, &buffer[Handle::read_handle & mask()], first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(data + first_chunk, &buffer[0], (len - first_chunk) * sizeof(T));
            Handle::read_handle = (Handle::read_handle + len) & mask();
        }else{
            if (size() < len)
                return false;
            const std::size_t first_chunk = std::min(capacity() - Handle::read_handle, len);
            std::memcpy(data, &buffer[Handle::read_handle], first_chunk * sizeof(T));
            if (len - first_chunk > 0)
                std::memcpy(data + first_chunk, &buffer[0], (len - first_chunk) * sizeof(T));
            Handle::read_handle = (Handle::read_handle + len) % capacity();
        }
        return true;
    }

    bool push(const T& value) {
        if (isFull()) {return false;}
        buffer[Handle::write_handle] = value;
        addWrite();
        return true;
    }
    void pushForce(const T& value) {
        if (isFull()) {addRead();}
        buffer[Handle::write_handle] = value;
        addWrite();
    }
    T pop() {
        if (isEmpty()) {return T();}
        const T& temp = buffer[Handle::read_handle];
        addRead();
        return temp;
    }
    void pop(T& value) {
        if (isEmpty()) {return;}
        value = buffer[Handle::read_handle];
        addRead();
    }

    void addWrite(std::size_t len = 1) {
        if constexpr (is2Power())
            Handle::write_handle = (Handle::write_handle + len) & mask();
        else
            Handle::write_handle = (Handle::write_handle + len) % capacity();
    }
    void addRead(std::size_t len = 1) {
        if constexpr(is2Power())
            Handle::read_handle = (Handle::read_handle + len) & mask();
        else
            Handle::read_handle = (Handle::read_handle + len) % capacity();
    }

    void reset() {
        Handle::read_handle = 0;
        Handle::write_handle = 0;
    }

private:
    [[nodiscard]] consteval static bool is2Power() {
        std::size_t count = 0;
        for (std::size_t i = 0; i < sizeof(std::size_t)*8; ++i) {
            if((N>>i)&1){count++;if (count==2) return false;}
        }
        return true;
    }

    T buffer[N];
};

}
