//
// Created by liaohy on 11/7/25.
//

#pragma once

// #include "config.h"
#include <cstdint>

namespace EP::Component {
namespace DetectDep {
    enum State : uint8_t{
        WORKING,
        MISSING,
        RECOVER,
        LOSE
    };
    using Fun = void(State s, void *);
    struct Function {
        Fun *func;
        void *param;
    };
}
template<auto sysTime>
class Detect {
    using fun = DetectDep::Fun;
    using function = DetectDep::Function;
    using State = DetectDep::State;
public:
    explicit Detect(const uint32_t maxInterval, const function& callback = {.func = nullptr, .param = nullptr}) : maxInterval(maxInterval), callback(callback) {}
    template<auto getSystime = sysTime>
    void update() {lastUpdate = getSystime();}
    template<auto getSystime = sysTime>
    State detect() {
        if (getSystime() - lastUpdate >= maxInterval)
            if (s == State::LOSE || s == State::MISSING) s = State::MISSING; else s = State::LOSE;
        else
            if (s == State::RECOVER || s == State::WORKING) s = State::WORKING; else s = State::RECOVER;
        return s;
    }
    template<auto getSystime = sysTime>
    void callbackFun() {
        if (getSystime() - lastUpdate >= maxInterval)
            if (s == State::LOSE || s == State::MISSING) s = State::MISSING; else s = State::LOSE;
        else
            if (s == State::RECOVER || s == State::WORKING) s = State::WORKING; else s = State::RECOVER;
        if (callback.func) callback.func(s, callback.param);
    }

private:
    uint32_t maxInterval = 0;
    uint32_t lastUpdate = 0;
    State s = State::WORKING;
    function callback;
};
}
