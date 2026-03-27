//
// Created by Administrator on 25-1-4.
//

/*
 * 在Release下经过测试直接调用arm_math库和数组，用时为A
 * 通过Matrix类封装ArmMathPl库和数组，用时为B几乎约为A左右
 * 通过Matrix类封装GeneralMathPl运算，用时为C=1.3B~1.4B左右
 */
#pragma once

#include <array>
#include "Config/Platform.h"
#include "Config/GeneralMathPl.h"
#include "Format.h"

namespace EP::Math {

/**
 * @brief A fixed-size matrix class template leveraging specific math platforms.
 * @tparam ROWS Number of rows
 * @tparam COLS Number of columns
 * @tparam pl Underlying math platform implementation (defaults to DefaultMathPl)
 */
template<uint32_t ROWS, uint32_t COLS, mathPl pl = DefaultMathPl>
class Matrix : private pl {
public:
    using BackendPl = pl;

    // --- Compile-time Traits ---
    consteval static bool is_point() { return (ROWS == 1 && COLS == 1); }
    consteval static bool is_row() { return (COLS != 1 && ROWS == 1); }
    consteval static bool is_col() { return (COLS == 1 && ROWS != 1); }
    consteval static bool is_vec() { return ((COLS == 1 && ROWS != 1) || (COLS != 1 && ROWS == 1)); }
    consteval static bool is_square() { return COLS == ROWS; }

    template<uint32_t R, uint32_t C, mathPl P>
    friend class Matrix;

public:
    // --- Static Factories ---
    /** @brief Creates a matrix filled with zeros */
    constexpr static Matrix zeros() noexcept;
    /** @brief Creates an identity matrix */
    constexpr static Matrix eyes() noexcept;

public:
    // --- Constructors & Assignment ---
    constexpr Matrix() noexcept;
    constexpr Matrix(const Matrix &other) noexcept;
    constexpr Matrix &operator=(const Matrix &other) noexcept;
    constexpr Matrix(Matrix &&other) = default;
    constexpr Matrix &operator=(Matrix &&other) = default;
    ~Matrix() = default;

    /** @brief Constructs from a 2D C-style array */
    constexpr explicit Matrix(const float (&arr)[ROWS][COLS]) noexcept;
    constexpr Matrix &operator=(const float (&arr)[ROWS][COLS]) noexcept;

    Matrix &operator=(std::nullptr_t) noexcept;
    constexpr bool operator==(std::nullptr_t) noexcept;
    constexpr bool operator!=(std::nullptr_t) noexcept;

    // --- Dimension Accessors ---
    consteval uint32_t getRow() { return ROWS; }
    consteval uint32_t getCol() { return COLS; }

    // --- Element Access ---
    /** @brief 2D index accessor (0-based) */
    float &operator()(const uint32_t row, const uint32_t col) noexcept;
    constexpr float operator()(const uint32_t row, const uint32_t col) const noexcept;

    // --- Basic Arithmetic Operators ---
    constexpr Matrix operator+(const Matrix &other) const noexcept;
    constexpr Matrix operator-(const Matrix &other) const noexcept;

    /** @brief Matrix multiplication */
    template<uint32_t cols>
    constexpr Matrix<ROWS, cols> operator*(const Matrix<COLS, cols> &other) const noexcept;

    // --- Scalar Arithmetic Operators ---
    constexpr Matrix operator*(float scale) const noexcept;
    constexpr friend Matrix<ROWS, COLS> operator*(float scale, const Matrix<ROWS, COLS> &other) noexcept {
        Matrix result;
        pl::matrix_scale(&other.matrix, scale, &result.matrix);
        return result;
    }
    constexpr Matrix operator/(const float scale) const noexcept;

    // --- Advanced Operations ---
    /** @brief Returns the transposed matrix */
    [[nodiscard]] constexpr Matrix<COLS, ROWS> transpose() const noexcept;
    constexpr void transpose(Matrix<COLS, ROWS> &mat) const;
    /** @brief Operator shorthand for transpose */
    constexpr Matrix<COLS, ROWS> operator~() const noexcept;

    constexpr Matrix operator-() const noexcept;

    void operator+=(const Matrix &other) noexcept;
    void operator-=(const Matrix &other) noexcept;
    void operator*=(const Matrix<COLS, COLS> &other) noexcept;
    void operator*=(float scale) noexcept;
    void operator/=(float scale) noexcept;

    /** @brief Calculates the inverse matrix, sets status */
    pl::status inv(Matrix &result) noexcept requires(is_square());
    /** @brief Returns the inverse matrix (or zeros if singular) */
    Matrix inv() noexcept requires(is_square());

    // --- In-Place Operations ---
    template<uint32_t common>
    void assignMultiply(const Matrix<ROWS, common> &first_matrix, const Matrix<common, COLS> &second_matrix) noexcept;
    void assignAdd(const Matrix &first_matrix, const Matrix &second_matrix) noexcept;
    void assignSub(const Matrix &first_matrix, const Matrix &second_matrix) noexcept;
    void assignScale(float scale) noexcept;
    pl::status assignInv(Matrix &scr_matrix) noexcept requires(is_square());

public:
    // --- Vector Specific Operations ---
    /** @brief Calculates the L2 norm (magnitude) of the vector */
    [[nodiscard]] float norm() const noexcept requires(is_vec());

    /** @brief Returns a normalized copy of the vector */
    [[nodiscard]] Matrix normalized() const noexcept requires(is_vec());

    /** @brief Vector cross product */
    Matrix operator^(const Matrix &other) const noexcept requires(is_vec() && (ROWS == 3 || COLS == 3));

    /** @brief Point specific constructor */
    explicit Matrix(float other) noexcept requires(is_point());

public:
    // --- Column Vector Operations ---
    constexpr explicit Matrix(const float (&arr)[ROWS]) noexcept requires(is_col());
    constexpr explicit Matrix(const std::array<float, ROWS> &arr) noexcept requires(is_col());

    /** @brief Vector dot product */
    constexpr float operator*(const Matrix &other) const noexcept requires(is_col());

    /** @brief 1D index accessor */
    float &operator[](uint32_t num) noexcept requires(is_col());
    constexpr float operator[](uint32_t num) const noexcept requires(is_col());
    float &operator()(uint32_t num) noexcept requires(is_col());
    constexpr float operator()(uint32_t num) const noexcept requires(is_col());

public:
    // --- Row Vector Operations ---
    constexpr explicit Matrix(const float (&arr)[COLS]) noexcept requires(is_row());
    constexpr explicit Matrix(const std::array<float, COLS> &arr) noexcept requires(is_row());

    /** @brief Vector dot product */
    constexpr float operator*(const Matrix &other) const noexcept requires(is_row());

    /** @brief 1D index accessor */
    float &operator[](uint32_t num) noexcept requires(is_row() || is_point());
    constexpr float operator[](uint32_t num) const noexcept requires(is_row() || is_point());
    float &operator()(uint32_t num) noexcept requires(is_row() || is_point());
    constexpr float operator()(uint32_t num) const noexcept requires(is_row() || is_point());

    // --- Formatting ---
    template<typename OStream>
    friend OStream &operator<<(OStream &os, const Matrix &mat) {
        os << "\n[";
        for (uint32_t i = 0; i < ROWS; ++i) {
            if (i > 0) os << "\n ";
            for (uint32_t j = 0; j < COLS; ++j) {
                os << " " << mat.data[i][j];
                if (j < COLS - 1) os << ",";
            }
            if (i < ROWS - 1) os << ";";
        }
        os << " ]";
        return os;
    }

    template<typename T>
    friend struct fmtter;

    float data[ROWS][COLS];

private:
    pl::MatrixView matrix{};
};

/** @brief Column vector alias */
template<uint32_t ROWS, mathPl pl = DefaultMathPl>
using ColVec = Matrix<ROWS, 1, pl>;

/** @brief Row vector alias */
template<uint32_t COLS, mathPl pl = DefaultMathPl>
using RowVec = Matrix<1, COLS, pl>;

}

namespace EP::Component {
template<uint32_t ROWS, uint32_t COLS, Math::mathPl pl>
struct fmtter<Math::Matrix<ROWS, COLS, pl> > {
    inline static void execute(char *&buf, const Math::Matrix<ROWS, COLS, pl> &value, const Feature &feature) {
        *buf++ = '\n';
        *buf++ = '[';
        for (uint32_t i = 0; i < ROWS; ++i) {
            if (i > 0) *buf++ = '\n';
            for (uint32_t j = 0; j < COLS; ++j) {
                fmtter<float>::execute(buf, value.data[i][j], feature);
                if (j < COLS - 1) {
                    *buf++ = ',';
                    *buf++ = ' ';
                }
            }
            if (i < ROWS - 1) *buf++ = ';';
        }
        *buf++ = ']';
    }
};
}

#include "Matrix.tpp"
