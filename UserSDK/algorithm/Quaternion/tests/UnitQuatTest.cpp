#include <gtest/gtest.h>
#include "Quaternion/UnitQuat.hpp"
#include <cmath>

using namespace EP::Math;

// Helper function to create base rotation matrices
Matrix<3, 3> Rx(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    float arr[3][3] = {
        {1, 0, 0},
        {0, c, -s},
        {0, s, c}
    };
    return Matrix<3, 3>(arr);
}

Matrix<3, 3> Ry(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    float arr[3][3] = {
        {c, 0, s},
        {0, 1, 0},
        {-s, 0, c}
    };
    return Matrix<3, 3>(arr);
}

Matrix<3, 3> Rz(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    float arr[3][3] = {
        {c, -s, 0},
        {s, c, 0},
        {0, 0, 1}
    };
    return Matrix<3, 3>(arr);
}

TEST(UnitQuatBasicTest, Initialization) {
    // Identity rotation
    UnitQuat q1(1.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(q1.norm(), 1.0f);

    // Axis-Angle initialization
    float arr[3] = {1.0f, 0.0f, 0.0f};
    ColVec<3> axis(arr);
    UnitQuat q2(static_cast<float>(M_PI), axis);
    EXPECT_FLOAT_EQ(q2.norm(), 1.0f);
    
    // Test normalization
    UnitQuat q3(2.0f, 0.0f, 0.0f, 0.0f);
    q3.normalized();
    EXPECT_FLOAT_EQ(q3.norm(), 1.0f);
}

TEST(UnitQuatMatrixTest, MatrixConversion) {
    // Generate a rotation matrix for ZYX
    auto R_base = Rz(0.3f) * Ry(0.2f) * Rx(0.1f);
    
    // formRotMatrix
    UnitQuat q = UnitQuat::formRotMatrix(R_base);
    EXPECT_NEAR(q.norm(), 1.0f, 1e-5);
    
    // rotMatrix
    auto R_out = q.rotMatrix();
    
    // Compare matrix elements
    for(uint32_t i=0; i<3; ++i) {
        for(uint32_t j=0; j<3; ++j) {
            EXPECT_NEAR(R_base(i, j), R_out(i, j), 1e-4);
        }
    }
}

TEST(UnitQuatEulerTest, TaitBryanAngles) {
    // EULER_ZYX means Rz(t1) * Ry(t2) * Rx(t3)
    float expected_z = 0.5f;
    float expected_y = 0.3f;
    float expected_x = -0.4f;
    
    auto R_zyx = Rz(expected_z) * Ry(expected_y) * Rx(expected_x);
    UnitQuat q_zyx = UnitQuat::formRotMatrix(R_zyx);
    
    // Test ZYX extraction
    auto angles_zyx = q_zyx.rotAngle<RotAngle::EULER_ZYX>().value();
    EXPECT_NEAR(angles_zyx[0], expected_z, 1e-4);
    EXPECT_NEAR(angles_zyx[1], expected_y, 1e-4);
    EXPECT_NEAR(angles_zyx[2], expected_x, 1e-4);
    
    // FIXED_XYZ is equivalent to EULER_ZYX, order should be X, Y, Z
    auto angles_fixed = q_zyx.rotAngle<RotAngle::FIXED_XYZ>().value();
    EXPECT_NEAR(angles_fixed[0], expected_x, 1e-4);
    EXPECT_NEAR(angles_fixed[1], expected_y, 1e-4);
    EXPECT_NEAR(angles_fixed[2], expected_z, 1e-4);
}

TEST(UnitQuatEulerTest, ProperEulerAngles) {
    // EULER_ZYZ means Rz(t1) * Ry(t2) * Rz(t3)
    float t1 = 0.6f;
    float t2 = 0.4f;
    float t3 = 0.2f;
    
    auto R_zyz = Rz(t1) * Ry(t2) * Rz(t3);
    UnitQuat q_zyz = UnitQuat::formRotMatrix(R_zyz);
    
    auto angles_zyz = q_zyz.rotAngle<RotAngle::EULER_ZYZ>().value();
    EXPECT_NEAR(angles_zyz[0], t1, 1e-4);
    EXPECT_NEAR(angles_zyz[1], t2, 1e-4);
    EXPECT_NEAR(angles_zyz[2], t3, 1e-4);
}

TEST(UnitQuatEulerTest, SingularityCheck) {
    // Gimbal lock situation for ZYX (Pitch = 90 degrees = PI/2)
    float p = static_cast<float>(M_PI / 2.0f); // 90 deg
    auto R_sing = Rz(0.0f) * Ry(p) * Rx(0.3f);
    UnitQuat q_sing = UnitQuat::formRotMatrix(R_sing);
    
    auto angles = q_sing.rotAngle<RotAngle::EULER_ZYX>();
    // When gimbal lock occurs, we expect nullopt
    EXPECT_FALSE(angles.has_value());
}

TEST(UnitQuatEulerTest, QuaternionSignInvariance) {
    const float axisArr[3] = {0.3f, -0.4f, 0.5f};
    ColVec<3> axis(axisArr);
    axis = axis.normalized();
    const float theta = 0.9f;

    const float w = std::cos(theta * 0.5f);
    const float s = std::sin(theta * 0.5f);

    UnitQuat q(theta, axis);
    UnitQuat qNeg(-w, -axis[0] * s, -axis[1] * s, -axis[2] * s);

    auto anglesQ = q.rotAngle<RotAngle::FIXED_XYZ>();
    auto anglesQNeg = qNeg.rotAngle<RotAngle::FIXED_XYZ>();

    ASSERT_TRUE(anglesQ.has_value());
    ASSERT_TRUE(anglesQNeg.has_value());

    EXPECT_NEAR((*anglesQNeg)[0], (*anglesQ)[0], 1e-4);
    EXPECT_NEAR((*anglesQNeg)[1], (*anglesQ)[1], 1e-4);
    EXPECT_NEAR((*anglesQNeg)[2], (*anglesQ)[2], 1e-4);
}
