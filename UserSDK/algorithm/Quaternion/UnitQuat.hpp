//
// Created by liaohy on 3/27/26.
//

#pragma once

#include <optional>
#include <array>
#include "Quaternion.hpp"

namespace EP::Math {

/**
 * @brief Represents the 24 possible sequences for Euler and Fixed axis rotations.
 *
 * For EULER_XYZ, the rotations are applied as Rz * Ry * Rx.
 * FIXED_XYZ is mathematically equivalent to EULER_ZYX.
 */
enum class RotAngle {
    EULER_XYZ, EULER_XZY, EULER_YXZ, EULER_YZX, EULER_ZXY, EULER_ZYX,
    EULER_XYX, EULER_XZX, EULER_YXY, EULER_YZY, EULER_ZXZ, EULER_ZYZ,
    FIXED_XYZ, FIXED_XZY, FIXED_YXZ, FIXED_YZX, FIXED_ZXY, FIXED_ZYX,
    FIXED_XYX, FIXED_XZX, FIXED_YXY, FIXED_YZY, FIXED_ZXZ, FIXED_ZYZ,
};

/**
 * @brief Unit Quaternion class for representing and manipulating 3D rotations.
 * Ensures the quaternion remains normalized and provides extensive conversions.
 */
class UnitQuat : public Quaternion {
public:
    // --- Constructors ---
    /** @brief Constructs a unit quaternion from an angle (radians) and an axis vector */
    constexpr UnitQuat(float theta, const Vec<3> &vec) : Quaternion(pl::cos(theta / 2), pl::sin(theta / 2) * vec) {};

    /** @brief Constructs from 4 individual components (w, x, y, z) */
    constexpr UnitQuat(float t1, float t2, float t3, float t4) : Quaternion(t1, t2, t3, t4) {};

    /** @brief Explicitly constructs from a general quaternion */
    constexpr explicit UnitQuat(const Quaternion &q) : Quaternion(q) {};

    // --- Basic Operations ---
    /** @brief Normalizes the quaternion in-place */
    constexpr void normalized();

    /** @brief Returns the inverse (conjugate) of this unit quaternion */
    constexpr UnitQuat inv() const;

    /** @brief Updates the quaternion based on a new angle, keeping the same axis */
    void update(float theta);

    // --- Conversions ---
    /** @brief Creates a unit quaternion from a 3x3 rotation matrix */
    static UnitQuat formRotMatrix(const Matrix<3, 3, pl> &r);

    /** @brief Converts the unit quaternion to a 3x3 rotation matrix */
    Matrix<3, 3, pl> rotMatrix() const;

    /** 
     * @brief Extracts the Euler/Fixed angles from the quaternion.
     * @tparam T The rotation sequence standard (e.g., EULER_ZYX)
     * @return std::array<float, 3> The angles strictly following the order of the axes in the enum name.
     */
    template<RotAngle T>
    std::optional<std::array<float, 3>> rotAngle() const;
};

} // namespace EP::Math

namespace EP::Component {

template<>
struct fmtter<Math::UnitQuat> {
    inline static void execute(char *&buf, const Math::UnitQuat &value, const Feature &feature) {
        *buf++ = '\n';
        *buf++ = '[';
        fmtter<float>::execute(buf, value.w, feature);
        fmtter<decltype(value.u)>::execute(buf, value.u, feature);
        *buf++ = ']';
    }
};

} // namespace EP::Component

#include "UnitQuat.tpp"
