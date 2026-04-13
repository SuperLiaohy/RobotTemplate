//
// Created by liaohy on 1/16/26.
//

#pragma once

#include <Format.h>

namespace EP::Component {
template<auto print, std::size_t bufLen = 1024>
class Logger {
    char buffer[bufLen];
    std::size_t mLen = 0;

public:
    Logger() = default;

    template<String auto fmt, typename... Args>
    void log(Args &&... args) {
        if constexpr (sizeof...(Args) > 0) {
            using Indices = std::make_index_sequence<sizeof...(Args)>;
            mLen += for_each_impl<fmt, Indices>::execute(buffer + mLen, std::forward<Args>(args)...);
        } else {
            memcpy(buffer + mLen, fmt.c_str(), fmt.len());
            mLen += fmt.len();
        }
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
