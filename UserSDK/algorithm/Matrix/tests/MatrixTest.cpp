#include <gtest/gtest.h>
#include "Matrix/Matrix.hpp"

using namespace EP::Math;

TEST(MatrixTraitsTest, StaticProperties) {
    EXPECT_TRUE((Matrix<1, 1>::is_point()));
    EXPECT_FALSE((Matrix<2, 2>::is_point()));

    EXPECT_TRUE((Matrix<1, 3>::is_row()));
    EXPECT_FALSE((Matrix<3, 1>::is_row()));

    EXPECT_TRUE((Matrix<3, 1>::is_col()));
    EXPECT_FALSE((Matrix<1, 3>::is_col()));

    EXPECT_TRUE((Matrix<3, 1>::is_vec()));
    EXPECT_TRUE((Matrix<1, 3>::is_vec()));
    EXPECT_FALSE((Matrix<3, 3>::is_vec()));

    EXPECT_TRUE((Matrix<4, 4>::is_square()));
    EXPECT_FALSE((Matrix<3, 4>::is_square()));
}

TEST(MatrixConstructorTest, Initialization) {
    auto m_zeros = Matrix<2, 2>::zeros();
    EXPECT_FLOAT_EQ(m_zeros(0, 0), 0.0f);
    EXPECT_FLOAT_EQ(m_zeros(1, 1), 0.0f);

    auto m_eyes = Matrix<3, 3>::eyes();
    EXPECT_FLOAT_EQ(m_eyes(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m_eyes(1, 1), 1.0f);
    EXPECT_FLOAT_EQ(m_eyes(0, 1), 0.0f);

    float arr[2][2] = {{1.0f, 2.0f}, {3.0f, 4.0f}};
    Matrix<2, 2> m_arr(arr);
    EXPECT_FLOAT_EQ(m_arr(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m_arr(1, 1), 4.0f);

    Matrix<2, 2> m_copy(m_arr);
    EXPECT_FLOAT_EQ(m_copy(0, 1), 2.0f);

    Matrix<2, 2> m_assign;
    m_assign = m_arr;
    EXPECT_FLOAT_EQ(m_assign(1, 0), 3.0f);

    m_assign = std::nullptr_t{};
    EXPECT_TRUE(m_assign == nullptr);
    EXPECT_FALSE(m_assign != nullptr);
}

TEST(MatrixAccessorTest, Elements) {
    Matrix<2, 2> mat;
    mat(0, 0) = 5.0f;
    mat(1, 1) = 10.0f;
    EXPECT_FLOAT_EQ(mat(0, 0), 5.0f);
    EXPECT_FLOAT_EQ(mat(1, 1), 10.0f);

    ColVec<3> cvec;
    cvec[0] = 1.0f;
    cvec[1] = 2.0f;
    cvec[2] = 3.0f;
    EXPECT_FLOAT_EQ(cvec(0), 1.0f);
    EXPECT_FLOAT_EQ(cvec[1], 2.0f);

    RowVec<2> rvec;
    rvec[0] = 4.0f;
    rvec[1] = 5.0f;
    EXPECT_FLOAT_EQ(rvec(0), 4.0f);
    EXPECT_FLOAT_EQ(rvec[1], 5.0f);
}

TEST(MatrixArithmeticTest, BasicMath) {
    float arr1[2][2] = {{1, 2}, {3, 4}};
    float arr2[2][2] = {{5, 6}, {7, 8}};
    Matrix<2, 2> m1(arr1);
    Matrix<2, 2> m2(arr2);

    auto m_add = m1 + m2;
    EXPECT_FLOAT_EQ(m_add(0, 0), 6.0f);
    EXPECT_FLOAT_EQ(m_add(1, 1), 12.0f);

    auto m_sub = m2 - m1;
    EXPECT_FLOAT_EQ(m_sub(0, 0), 4.0f);
    EXPECT_FLOAT_EQ(m_sub(1, 1), 4.0f);

    auto m_scale1 = m1 * 2.0f;
    EXPECT_FLOAT_EQ(m_scale1(0, 0), 2.0f);
    EXPECT_FLOAT_EQ(m_scale1(1, 1), 8.0f);

    auto m_scale2 = 3.0f * m1;
    EXPECT_FLOAT_EQ(m_scale2(0, 0), 3.0f);
    EXPECT_FLOAT_EQ(m_scale2(1, 1), 12.0f);

    auto m_div = m1 / 2.0f;
    EXPECT_FLOAT_EQ(m_div(0, 0), 0.5f);
    EXPECT_FLOAT_EQ(m_div(1, 1), 2.0f);

    m1 += m2;
    EXPECT_FLOAT_EQ(m1(0, 0), 6.0f);

    m1 -= m2;
    EXPECT_FLOAT_EQ(m1(0, 0), 1.0f);

    m1 *= 2.0f;
    EXPECT_FLOAT_EQ(m1(0, 0), 2.0f);

    m1 /= 2.0f;
    EXPECT_FLOAT_EQ(m1(0, 0), 1.0f);
}

TEST(MatrixMultiplicationTest, DotAndCross) {
    float arr1[2][3] = {{1, 2, 3}, {4, 5, 6}};
    float arr2[3][2] = {{7, 8}, {9, 10}, {11, 12}};
    Matrix<2, 3> m1(arr1);
    Matrix<3, 2> m2(arr2);

    auto m_mul = m1 * m2;
    // 1*7 + 2*9 + 3*11 = 7 + 18 + 33 = 58
    EXPECT_FLOAT_EQ(m_mul(0, 0), 58.0f);
    // 4*8 + 5*10 + 6*12 = 32 + 50 + 72 = 154
    EXPECT_FLOAT_EQ(m_mul(1, 1), 154.0f);

    Matrix<2, 2> m3 = Matrix<2, 2>::eyes();
    Matrix<2, 2> m4 = Matrix<2, 2>::eyes() * 2.0f;
    m3 *= m4;
    EXPECT_FLOAT_EQ(m3(0, 0), 2.0f);
    EXPECT_FLOAT_EQ(m3(1, 1), 2.0f);
}

TEST(MatrixAdvancedTest, TransposeAndInverse) {
    float arr1[2][3] = {{1, 2, 3}, {4, 5, 6}};
    Matrix<2, 3> m1(arr1);

    auto m1_t = m1.transpose();
    EXPECT_FLOAT_EQ(m1_t(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m1_t(0, 1), 4.0f);
    EXPECT_FLOAT_EQ(m1_t(1, 0), 2.0f);
    EXPECT_FLOAT_EQ(m1_t(2, 1), 6.0f);

    auto m1_t2 = ~m1;
    EXPECT_FLOAT_EQ(m1_t2(2, 1), 6.0f);

    auto m_neg = -m1;
    EXPECT_FLOAT_EQ(m_neg(0, 0), -1.0f);

    float arr_sq[2][2] = {{4, 7}, {2, 6}};
    Matrix<2, 2> m_sq(arr_sq);
    auto m_inv = m_sq.inv();

    // det = 24 - 14 = 10
    // inv = [0.6, -0.7; -0.2, 0.4]
    EXPECT_NEAR(m_inv(0, 0), 0.6f, 1e-5);
    EXPECT_NEAR(m_inv(0, 1), -0.7f, 1e-5);
    EXPECT_NEAR(m_inv(1, 0), -0.2f, 1e-5);
    EXPECT_NEAR(m_inv(1, 1), 0.4f, 1e-5);

    Matrix<2, 2> inv_res;
    EXPECT_EQ(m_sq.inv(inv_res), GeneralMathPl::SUCCESS);
    EXPECT_NEAR(inv_res(0, 0), 0.6f, 1e-5);
}

TEST(VectorSpecificTest, VectorMath) {
    float arr1[3] = {1.0f, 2.0f, 2.0f};
    ColVec<3> v1(arr1);

    EXPECT_FLOAT_EQ(v1.norm(), 3.0f);

    auto v1_norm = v1.normalized();
    EXPECT_FLOAT_EQ(v1_norm[0], 1.0f / 3.0f);
    EXPECT_FLOAT_EQ(v1_norm[1], 2.0f / 3.0f);

    float arr2[3] = {2.0f, 1.0f, 2.0f};
    ColVec<3> v2(arr2);

    auto v_cross = v1 ^ v2;
    // v1 = [1, 2, 2]
    // v2 = [2, 1, 2]
    // cross = [2*2 - 2*1, 2*2 - 1*2, 1*1 - 2*2] = [2, 2, -3]
    EXPECT_FLOAT_EQ(v_cross[0], 2.0f);
    EXPECT_FLOAT_EQ(v_cross[1], 2.0f);
    EXPECT_FLOAT_EQ(v_cross[2], -3.0f);

    // Inner product
    float inner2 = v1 * v2; // operator*(const Matrix &other) for col vec computes dot product
    EXPECT_FLOAT_EQ(inner2, 1*2 + 2*1 + 2*2); // 2+2+4 = 8
}

TEST(MatrixInPlaceAssignTest, InPlaceOperations) {
    float arr1[2][2] = {{1, 2}, {3, 4}};
    float arr2[2][2] = {{5, 6}, {7, 8}};
    
    Matrix<2, 2> m1(arr1);
    Matrix<2, 2> m2(arr2);
    Matrix<2, 2> result;

    // 1. Test assignAdd
    result.assignAdd(m1, m2);
    EXPECT_FLOAT_EQ(result(0, 0), 6.0f);
    EXPECT_FLOAT_EQ(result(1, 1), 12.0f);

    // 2. Test assignSub
    result.assignSub(m2, m1);
    EXPECT_FLOAT_EQ(result(0, 0), 4.0f);
    EXPECT_FLOAT_EQ(result(1, 1), 4.0f);

    // 3. Test assignScale
    result = m1; // Restore initial values {{1, 2}, {3, 4}}
    result.assignScale(3.0f);
    EXPECT_FLOAT_EQ(result(0, 0), 3.0f);
    EXPECT_FLOAT_EQ(result(0, 1), 6.0f);
    EXPECT_FLOAT_EQ(result(1, 1), 12.0f);

    // 4. Test assignMultiply
    float arr3[2][3] = {{1, 2, 3}, {4, 5, 6}};
    float arr4[3][2] = {{7, 8}, {9, 10}, {11, 12}};
    Matrix<2, 3> m3(arr3);
    Matrix<3, 2> m4(arr4);
    Matrix<2, 2> mul_result;
    mul_result.assignMultiply(m3, m4);
    // 1*7 + 2*9 + 3*11 = 58
    EXPECT_FLOAT_EQ(mul_result(0, 0), 58.0f);
    // 4*8 + 5*10 + 6*12 = 154
    EXPECT_FLOAT_EQ(mul_result(1, 1), 154.0f);

    // 5. Test assignInv
    float arr_sq[2][2] = {{4, 7}, {2, 6}};
    Matrix<2, 2> m_sq(arr_sq);
    Matrix<2, 2> inv_result;
    
    auto status = inv_result.assignInv(m_sq);
    EXPECT_EQ(status, GeneralMathPl::SUCCESS);
    // det = 24 - 14 = 10, inv = [0.6, -0.7; -0.2, 0.4]
    EXPECT_NEAR(inv_result(0, 0), 0.6f, 1e-5);
    EXPECT_NEAR(inv_result(0, 1), -0.7f, 1e-5);
    EXPECT_NEAR(inv_result(1, 0), -0.2f, 1e-5);
    EXPECT_NEAR(inv_result(1, 1), 0.4f, 1e-5);
}
