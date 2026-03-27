//
// Created by liaohy on 25-3-13.
//

#pragma once

#include "../Matrix/Matrix.hpp"

namespace EP::Math {
template<mathPl pl = DefaultMathPl>
class Quaternion {
public:
    template<uint32_t T>
    using Vec = ColVec<T, pl>;

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

enum class RotAngle {
    EULER_XYZ,
    EULER_XZY,
    EULER_YXZ,
    EULER_YZX,
    EULER_ZXY,
    EULER_ZYX,
    EULER_XYX,
    EULER_XZX,
    EULER_YXY,
    EULER_YZY,
    EULER_ZXZ,
    EULER_ZYZ,
    FIXED_XYZ,
    FIXED_XZY,
    FIXED_YXZ,
    FIXED_YZX,
    FIXED_ZXY,
    FIXED_ZYX,
    FIXED_XYX,
    FIXED_XZX,
    FIXED_YXY,
    FIXED_YZY,
    FIXED_ZXZ,
    FIXED_ZYZ,
};

template<mathPl pl = DefaultMathPl>
class UnitQuat : public Quaternion<pl> {
public:
    template<uint32_t T>
    using Vec = Quaternion<pl>::template Vec<T>;

    constexpr UnitQuat(float theta, const Vec<3> &vec) : Quaternion<pl>(pl::cos(theta / 2), pl::sin(theta / 2) * vec) {};

    constexpr UnitQuat(float t1, float t2, float t3, float t4) : Quaternion<pl>(t1, t2, t3, t4) {};

    constexpr explicit UnitQuat(const Quaternion<pl> &q) : Quaternion<pl>(q) {};

    constexpr void normalized() {
        auto size = pl::sqrtf(this->w * this->w + (this->u * this->u));
        this->operator/=(size);
    }

    constexpr UnitQuat inv() const { return UnitQuat(this->conj()); }

    void update(float theta) {
        this->w = pl::cos(theta / 2);
        this->u *= pl::sin(theta / 2);
    };

    static UnitQuat formRotMatrix(const Matrix<3, 3, pl> &r) {
        auto w = 0.5f*pl::sqrtf(1+r(0,0)+r(1,1)+r(2,2));
        return UnitQuat{
            w,
            (r(2,1)-r(1,2))/w,
            (r(0,2)-r(2,0))/w,
            (r(1,0)-r(0,1))/w
        };
    }

    Matrix<3, 3, pl> rotMatrix() {
        return Matrix<3, 3, pl>{{
            {
                1 - 2 * (this->u[1] * this->u[1] + this->u[2] * this->u[2]),
                2 * (this->u[0] * this->u[1] - this->u[2] * this->w),
                2 * (this->u[0] * this->u[2] + this->u[1] * this->w)
            },
            {
                2 * (this->u[0] * this->u[1] + this->u[2] * this->w),
                1 - 2 * (this->u[0] * this->u[0] + this->u[2] * this->u[2]),
                2 * (this->u[1] * this->u[2] - this->u[0] * this->w)
            },
            {
                2 * (this->u[0] * this->u[2] - this->u[1] * this->w),
                2 * (this->u[1] * this->u[2] + this->u[0] * this->w),
                1 - 2 * (this->u[0] * this->u[0] + this->u[1] * this->u[1])
            }
        }};
    }

    template<RotAngle T>
    std::array<float, 3> rotAngle() {
        if constexpr (T==RotAngle::EULER_ZYX || T == RotAngle::FIXED_XYZ) {
            float b =  pl::atan2(-2 * (this->u[0] * this->u[2] - this->u[1] * this->w), pl::sqrtf((1 - 2 * (this->u[1] * this->u[1] + this->u[2] * this->u[2]))*(1 - 2 * (this->u[1] * this->u[1] + this->u[2] * this->u[2])+(2 * (this->u[0] * this->u[1] + this->u[2] * this->w)*(2 * (this->u[0] * this->u[1] + this->u[2] * this->w))))));
            float cb = pl::cos(b);
            return {
                b,
                pl::atan2((2 * (this->u[0] * this->u[1] + this->u[2] * this->w))/cb,1 - 2 * (this->u[1] * this->u[1] + this->u[2] * this->u[2])/cb),
                pl::atan2((2 * (this->u[1] * this->u[2] + this->u[0] * this->w))/cb,(1 - 2 * (this->u[0] * this->u[0] + this->u[1] * this->u[1]))/cb)
            };
        } else if constexpr (T==RotAngle::FIXED_ZYX) {
            static_assert(false,"undefine type");
            return {};
        } else {
            static_assert(false,"undefine type");
            return {};
        }
    };
};
}

namespace EP::Component {
template<Math::mathPl pl>
struct fmtter<Math::Quaternion<pl>> {
    inline static void execute(char *&buf, const Math::Quaternion<pl> &value, const Feature &feature) {
        *buf++ = '\n';
        *buf++ = '[';
        fmtter<float>::execute(buf,value.w,feature);
        fmtter<decltype(value.u)>::execute(buf,value.u,feature);
        *buf++ = ']';
    }
};
template<Math::mathPl pl>
struct fmtter<Math::UnitQuat<pl>> {
    inline static void execute(char *&buf, const Math::UnitQuat<pl> &value, const Feature &feature) {
        *buf++ = '\n';
        *buf++ = '[';
        fmtter<float>::execute(buf,value.w,feature);
        fmtter<decltype(value.u)>::execute(buf,value.u,feature);
        *buf++ = ']';
    }
};
}