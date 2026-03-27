//
// Created by liaohy on 1/16/26.
//

#pragma once

#include <Format.h>

namespace EP::Component {
template<auto print>
class Logger {
    char buffer[1024];
    std::size_t mLen = 0;

public:
    Logger() = default;

    template<String auto fmt, typename... Args>
    void log(Args &&... args) {
        using Indices = std::make_index_sequence<sizeof...(Args)>;
        mLen += for_each_impl<fmt, Indices>::execute(buffer + mLen, std::forward<Args>(args)...);
    }

    void log(const char *msg, uint32_t len) {
        memcpy(buffer + mLen, msg, len);
        mLen += len;
    }

    void flush() {
        print(reinterpret_cast<uint8_t *>(buffer), mLen);
        mLen = 0;
    }
};
}
