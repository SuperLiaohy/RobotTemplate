//
// Created by liaohy on 25-3-13.
//

#pragma once

#include "../Matrix/Matrix.hpp"

namespace EP::Math {
class Quaternion {
protected:
    using pl = Matrix<1,1>::BackendPl;
public:
    template<uint32_t T>
    using Vec = ColVec<T>;

    static constexpr Quaternion zero() { return Quaternion{0, 0, 0, 0}; }

    static constexpr Quaternion init() { return Quaternion{1, 0, 0, 0}; }

public:
    constexpr Quaternion(float w, float x, float y, float z) : w(w), u(Vec<3>({x, y, z})) {
    };

    constexpr Quaternion(float w, const Vec<3> &u) : w(w), u(u) {
    };

    [[nodiscard]] constexpr Quaternion derivative(float wx, float wy, float wz) const {
        return Quaternion(0, wx * 0.5f, wy * 0.5f, wz * 0.5f) * (*this);
    };

    constexpr Quaternion operator*(const Quaternion &other) const {
        return Quaternion{other.w * w - (other.u * u), other.w * u + w * other.u + (u ^ other.u)};
    };

    constexpr Quaternion operator*(float scale) const {
        return Quaternion{w * scale, u * scale};
    };

    constexpr Quaternion operator/(float scale) const {
        return Quaternion{w / scale, u / scale};
    };

    friend constexpr Quaternion operator*(float scale, const Quaternion &other) {
        return Quaternion{scale * other.w, other.u * scale};
    };

    constexpr Quaternion operator+(const Quaternion &other) const {
        return Quaternion{w + other.w, u + other.u};
    }

    constexpr Quaternion operator-(const Quaternion &other) const {
        return Quaternion{w - other.w, u - other.u};
    }

    constexpr Quaternion operator-() const {
        return Quaternion{-w, -u[0], -u[1], -u[2]};
    }

    constexpr Quaternion conj() const {
        return Quaternion{w, -u[0], -u[1], -u[2]};
    }

    constexpr Quaternion operator~() const {
        return Quaternion{w, -u[0], -u[1], -u[2]};
    }

    constexpr void operator/=(float scale) {
        w /= scale;
        u /= scale;
    };

    constexpr Quaternion normalized() const {
        auto size = pl::sqrtf(w * w + (u * u));
        if (size < 1e-7) { return zero(); }
        return Quaternion{w / size, u / size};
    }
    [[nodiscard]] constexpr float norm() const {
        return pl::sqrtf(w * w + (u * u));
    }

    constexpr Quaternion inv() const {
        auto size = pl::sqrtf(w * w + (u * u));
        if (size < 1e-7) { return zero(); }
        return Quaternion{w / size, -u / size};
    }

    template<typename OStream>
    friend OStream &operator<<(OStream &os, const Quaternion &mat) {
        os << "\n[";
        os << " " << mat.w << ",";
        os << " " << mat.u;
        os << " ]";
        return os;
    }
    template<typename T>
    friend struct EP::Component::fmtter;

protected:
    float w;
    Vec<3> u;
};

}
namespace EP::Component {

template<>
struct fmtter<Math::Quaternion> {
    inline static void execute(char *&buf, const Math::Quaternion &value, const Feature &feature) {
        *buf++ = '\n';
        *buf++ = '[';
        fmtter<float>::execute(buf,value.w,feature);
        fmtter<decltype(value.u)>::execute(buf,value.u,feature);
        *buf++ = ']';
    }
};
}

