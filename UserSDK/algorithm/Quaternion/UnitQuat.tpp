#pragma once

namespace EP::Math {

constexpr void UnitQuat::normalized() {
    auto size = pl::sqrtf(this->w * this->w + (this->u * this->u));
    if (size > 1e-7f) {
        this->operator/=(size);
    }
}

constexpr UnitQuat UnitQuat::inv() const {
    return UnitQuat(this->conj());
}

inline void UnitQuat::update(float theta) {
    auto size = pl::sqrtf(this->u * this->u);
    if (size > 1e-7f) {
        this->w = pl::cos(theta / 2.0f);
        this->u = (this->u / size) * pl::sin(theta / 2.0f);
    } else {
        // If axis is zero, arbitrary axis with theta update
        this->w = pl::cos(theta / 2.0f);
    }
}

inline UnitQuat UnitQuat::formRotMatrix(const Matrix<3, 3, UnitQuat::pl> &R) {
    float tr = R(0, 0) + R(1, 1) + R(2, 2);
    float qw, qx, qy, qz;
    if (tr > 0.0f) {
        float S = pl::sqrtf(tr + 1.0f) * 2.0f; 
        qw = 0.25f * S;
        qx = (R(2, 1) - R(1, 2)) / S;
        qy = (R(0, 2) - R(2, 0)) / S;
        qz = (R(1, 0) - R(0, 1)) / S;
    } else if ((R(0, 0) > R(1, 1)) && (R(0, 0) > R(2, 2))) {
        float S = pl::sqrtf(1.0f + R(0, 0) - R(1, 1) - R(2, 2)) * 2.0f;
        qw = (R(2, 1) - R(1, 2)) / S;
        qx = 0.25f * S;
        qy = (R(0, 1) + R(1, 0)) / S;
        qz = (R(0, 2) + R(2, 0)) / S;
    } else if (R(1, 1) > R(2, 2)) {
        float S = pl::sqrtf(1.0f + R(1, 1) - R(0, 0) - R(2, 2)) * 2.0f;
        qw = (R(0, 2) - R(2, 0)) / S;
        qx = (R(0, 1) + R(1, 0)) / S;
        qy = 0.25f * S;
        qz = (R(1, 2) + R(2, 1)) / S;
    } else {
        float S = pl::sqrtf(1.0f + R(2, 2) - R(0, 0) - R(1, 1)) * 2.0f;
        qw = (R(1, 0) - R(0, 1)) / S;
        qx = (R(0, 2) + R(2, 0)) / S;
        qy = (R(1, 2) + R(2, 1)) / S;
        qz = 0.25f * S;
    }
    return UnitQuat{qw, qx, qy, qz};
}

inline Matrix<3, 3, UnitQuat::pl> UnitQuat::rotMatrix() const {
    float q0 = this->w;
    float q1 = this->u[0];
    float q2 = this->u[1];
    float q3 = this->u[2];

    return Matrix<3, 3, pl>{{
        {
            1.0f - 2.0f * (q2 * q2 + q3 * q3),
            2.0f * (q1 * q2 - q0 * q3),
            2.0f * (q1 * q3 + q0 * q2)
        },
        {
            2.0f * (q1 * q2 + q0 * q3),
            1.0f - 2.0f * (q1 * q1 + q3 * q3),
            2.0f * (q2 * q3 - q0 * q1)
        },
        {
            2.0f * (q1 * q3 - q0 * q2),
            2.0f * (q2 * q3 + q0 * q1),
            1.0f - 2.0f * (q1 * q1 + q2 * q2)
        }
    }};
}
template<RotAngle T>
std::optional<std::array<float, 3>> UnitQuat::rotAngle() const {
    UnitQuat q = *this;
    q.normalized();
    if (q.w < 0.0f) {
        q = UnitQuat{-q.w, -q.u[0], -q.u[1], -q.u[2]};
    }

    auto R = q.rotMatrix();
    float theta1 = 0, theta2 = 0, theta3 = 0;

    auto clampUnit = [](float v) {
        if (v > 1.0f) { return 1.0f; }
        if (v < -1.0f) { return -1.0f; }
        return v;
    };

    // Equivalency mapping: FIXED_ABC == EULER_CBA
    constexpr RotAngle seq = 
        (T == RotAngle::FIXED_XYZ) ? RotAngle::EULER_ZYX :
        (T == RotAngle::FIXED_XZY) ? RotAngle::EULER_YZX :
        (T == RotAngle::FIXED_YXZ) ? RotAngle::EULER_ZXY :
        (T == RotAngle::FIXED_YZX) ? RotAngle::EULER_XZY :
        (T == RotAngle::FIXED_ZXY) ? RotAngle::EULER_YXZ :
        (T == RotAngle::FIXED_ZYX) ? RotAngle::EULER_XYZ :
        (T == RotAngle::FIXED_XYX) ? RotAngle::EULER_XYX :
        (T == RotAngle::FIXED_XZX) ? RotAngle::EULER_XZX :
        (T == RotAngle::FIXED_YXY) ? RotAngle::EULER_YXY :
        (T == RotAngle::FIXED_YZY) ? RotAngle::EULER_YZY :
        (T == RotAngle::FIXED_ZXZ) ? RotAngle::EULER_ZXZ :
        (T == RotAngle::FIXED_ZYZ) ? RotAngle::EULER_ZYZ : T;

    // Helper to check for singularity near +/- 90 degrees or 0/180 degrees
    auto isSingular = [](float val) { return std::abs(std::abs(val) - 1.0f) < 1e-5f; };

    if constexpr (seq == RotAngle::EULER_ZYX) { // z->y->x (Yaw, Pitch, Roll)
        const float r20 = clampUnit(R(2, 0));
        theta2 = pl::asin(-r20);
        if (!isSingular(r20)) {
            theta1 = pl::atan2(R(1, 0), R(0, 0));
            theta3 = pl::atan2(R(2, 1), R(2, 2));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_XYZ) { // x->y->z
        const float r02 = clampUnit(R(0, 2));
        theta2 = pl::asin(r02);
        if (!isSingular(r02)) {
            theta1 = pl::atan2(-R(1, 2), R(2, 2));
            theta3 = pl::atan2(-R(0, 1), R(0, 0));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_YXZ) { // y->x->z
        const float r12 = clampUnit(R(1, 2));
        theta2 = pl::asin(-r12);
        if (!isSingular(r12)) {
            theta1 = pl::atan2(R(0, 2), R(2, 2));
            theta3 = pl::atan2(R(1, 0), R(1, 1));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_YZX) { // y->z->x
        const float r10 = clampUnit(R(1, 0));
        theta2 = pl::asin(r10);
        if (!isSingular(r10)) {
            theta1 = pl::atan2(-R(2, 0), R(0, 0));
            theta3 = pl::atan2(-R(1, 2), R(1, 1));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_ZXY) { // z->x->y
        const float r21 = clampUnit(R(2, 1));
        theta2 = pl::asin(r21);
        if (!isSingular(r21)) {
            theta1 = pl::atan2(-R(0, 1), R(1, 1));
            theta3 = pl::atan2(-R(2, 0), R(2, 2));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_XZY) { // x->z->y
        const float r01 = clampUnit(R(0, 1));
        theta2 = pl::asin(-r01);
        if (!isSingular(r01)) {
            theta1 = pl::atan2(R(2, 1), R(1, 1));
            theta3 = pl::atan2(R(0, 2), R(0, 0));
        } else {
            return std::nullopt;
        }
    } 
    // --- Proper Euler Angles ---
    else if constexpr (seq == RotAngle::EULER_ZYZ) { // z->y->z
        const float r22 = clampUnit(R(2, 2));
        theta2 = pl::acos(r22);
        if (!isSingular(r22)) {
            theta1 = pl::atan2(R(1, 2), R(0, 2));
            theta3 = pl::atan2(R(2, 1), -R(2, 0));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_ZXZ) { // z->x->z
        const float r22 = clampUnit(R(2, 2));
        theta2 = pl::acos(r22);
        if (!isSingular(r22)) {
            theta1 = pl::atan2(R(0, 2), -R(1, 2));
            theta3 = pl::atan2(R(2, 0), R(2, 1));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_YXY) { // y->x->y
        const float r11 = clampUnit(R(1, 1));
        theta2 = pl::acos(r11);
        if (!isSingular(r11)) {
            theta1 = pl::atan2(R(0, 1), R(2, 1));
            theta3 = pl::atan2(R(1, 0), -R(1, 2));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_YZY) { // y->z->y
        const float r11 = clampUnit(R(1, 1));
        theta2 = pl::acos(r11);
        if (!isSingular(r11)) {
            theta1 = pl::atan2(R(2, 1), -R(0, 1));
            theta3 = pl::atan2(R(1, 2), R(1, 0));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_XYX) { // x->y->x
        const float r00 = clampUnit(R(0, 0));
        theta2 = pl::acos(r00);
        if (!isSingular(r00)) {
            theta1 = pl::atan2(R(1, 0), -R(2, 0));
            theta3 = pl::atan2(R(0, 1), R(0, 2));
        } else {
            return std::nullopt;
        }
    } else if constexpr (seq == RotAngle::EULER_XZX) { // x->z->x
        const float r00 = clampUnit(R(0, 0));
        theta2 = pl::acos(r00);
        if (!isSingular(r00)) {
            theta1 = pl::atan2(R(2, 0), R(1, 0));
            theta3 = pl::atan2(R(0, 2), -R(0, 1));
        } else {
            return std::nullopt;
        }
    } else {
        static_assert(false, "undefined type");
    }

    constexpr bool isFixed = (T >= RotAngle::FIXED_XYZ && T <= RotAngle::FIXED_ZYZ);
    if constexpr (isFixed) {
        return std::array<float, 3>{theta3, theta2, theta1};
    } else {
        return std::array<float, 3>{theta1, theta2, theta3};
    }
}

} // namespace EP::Math
