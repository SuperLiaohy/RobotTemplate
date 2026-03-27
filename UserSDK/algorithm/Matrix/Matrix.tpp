#pragma once

namespace EP::Math {

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::zeros() noexcept {
    Matrix result{};
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            result.data[i][j] = 0;
        }
    }
    return result; // fixed Matrix{} to result
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::eyes() noexcept {
    Matrix result;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (i == j)
                result.data[i][j] = 1;
            else
                result.data[i][j] = 0;
        }
    }
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix() noexcept {
    pl::matrix_init(&matrix, ROWS, COLS, reinterpret_cast<float32_t *>(data));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const Matrix &other) noexcept : Matrix() {
    memcpy(data, other.data, ROWS * COLS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> &Matrix<ROWS, COLS, pl>::operator=(const Matrix &other) noexcept {
    if (&other == this) { return *this; }
    pl::matrix_init(&matrix, ROWS, COLS, reinterpret_cast<float32_t *>(data));
    memcpy(data, other.data, ROWS * COLS * sizeof(float));
    return *this;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const float (&arr)[ROWS][COLS]) noexcept : Matrix() {
    memcpy(this->data, arr, ROWS * COLS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> &Matrix<ROWS, COLS, pl>::operator=(const float (&arr)[ROWS][COLS]) noexcept {
    pl::matrix_init(&matrix, ROWS, COLS, reinterpret_cast<float32_t *>(data));
    memcpy(data, arr, ROWS * COLS * sizeof(float));
    return *this;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
Matrix<ROWS, COLS, pl> &Matrix<ROWS, COLS, pl>::operator=(std::nullptr_t) noexcept {
    matrix.pData = nullptr;
    return *this;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr bool Matrix<ROWS, COLS, pl>::operator==(std::nullptr_t) noexcept { return matrix.pData == nullptr; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr bool Matrix<ROWS, COLS, pl>::operator!=(std::nullptr_t) noexcept { return matrix.pData != nullptr; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float &Matrix<ROWS, COLS, pl>::operator()(const uint32_t row, const uint32_t col) noexcept { return data[row][col]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator()(const uint32_t row, const uint32_t col) const noexcept { return data[row][col]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator+(const Matrix &other) const noexcept  {
    Matrix result;
    pl::matrix_add(&matrix, &other.matrix, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator-(const Matrix &other) const noexcept  {
    Matrix result;
    pl::matrix_sub(&matrix, &other.matrix, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
template<uint32_t cols>
constexpr Matrix<ROWS, cols> Matrix<ROWS, COLS, pl>::operator*(const Matrix<COLS, cols> &other) const noexcept  {
    Matrix<ROWS, cols> result;
    pl::matrix_mul(&matrix, &other.matrix, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator*(float scale) const noexcept  {
    Matrix result;
    pl::matrix_scale(&matrix, scale, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator/(const float scale) const noexcept {
    Matrix result;
    pl::matrix_scale(&matrix, 1.0f / scale, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<COLS, ROWS> Matrix<ROWS, COLS, pl>::transpose() const noexcept {
    Matrix<COLS, ROWS> result;
    pl::matrix_trans(&matrix, &result.matrix);
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr void Matrix<ROWS, COLS, pl>::transpose(Matrix<COLS, ROWS> &mat) const {
    pl::matrix_trans(&matrix, &mat.matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<COLS, ROWS> Matrix<ROWS, COLS, pl>::operator~() const noexcept { return transpose(); }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator-() const noexcept { return *this * -1.0f; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::operator+=(const Matrix &other) noexcept {
    pl::matrix_add(&matrix, &other.matrix, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::operator-=(const Matrix &other) noexcept {
    pl::matrix_sub(&matrix, &other.matrix, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::operator*=(const Matrix<COLS, COLS> &other) noexcept {
    auto result = *this * other;
    *this = result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::operator*=(float scale) noexcept {
    pl::matrix_scale(&matrix, scale, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::operator/=(float scale) noexcept {
    pl::matrix_scale(&matrix, 1.0f / scale, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
inline pl::status Matrix<ROWS, COLS, pl>::inv(Matrix &result) noexcept requires(is_square()) {
    Matrix matrix_bak = *this;
    typename pl::status ret = pl::matrix_inverse(&matrix_bak.matrix, &result.matrix);
    return ret;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
inline Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::inv() noexcept requires(is_square()) {
    Matrix result;
    Matrix matrix_bak = *this;
    typename pl::status ret = pl::matrix_inverse(&matrix_bak.matrix, &result.matrix);
    return ret == pl::status::SUCCESS ? result : zeros();
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
template<uint32_t common>
void Matrix<ROWS, COLS, pl>::assignMultiply(const Matrix<ROWS, common> &first_matrix, const Matrix<common, COLS> &second_matrix) noexcept {
    pl::matrix_mul(&first_matrix.matrix, &second_matrix.matrix, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::assignAdd(const Matrix &first_matrix, const Matrix &second_matrix) noexcept {
    pl::matrix_add(&first_matrix.matrix, &second_matrix.matrix, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::assignSub(const Matrix &first_matrix, const Matrix &second_matrix) noexcept {
    pl::matrix_sub(&first_matrix.matrix, &second_matrix.matrix, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
void Matrix<ROWS, COLS, pl>::assignScale(float scale) noexcept {
    pl::matrix_scale(&matrix, scale, &matrix);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
pl::status Matrix<ROWS, COLS, pl>::assignInv(Matrix &scr_matrix) noexcept requires(is_square()) {
    typename pl::status ret = pl::matrix_inverse(&scr_matrix.matrix, &matrix);
    return ret;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float Matrix<ROWS, COLS, pl>::norm() const noexcept requires(is_vec()) {
    if constexpr (ROWS == 1) {
        return pl::sqrtf((*this * this->transpose())[0]);
    } else {
        return pl::sqrtf((this->transpose() * *this)[0]);
    }
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::normalized() const noexcept requires(is_vec()) {
    return (*this / norm());
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
Matrix<ROWS, COLS, pl> Matrix<ROWS, COLS, pl>::operator^(const Matrix &other) const noexcept requires(is_vec() && (ROWS == 3 || COLS == 3)) {
    Matrix result;
    const auto &result_array = reinterpret_cast<float *>(result.data);
    const auto &other_array = reinterpret_cast<const float *>(other.data);
    const auto &array = reinterpret_cast<const float *>(data);
    result_array[0] = array[1] * other_array[2] - array[2] * other_array[1];
    result_array[1] = array[2] * other_array[0] - array[0] * other_array[2];
    result_array[2] = array[0] * other_array[1] - array[1] * other_array[0];
    return result;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
Matrix<ROWS, COLS, pl>::Matrix(float other) noexcept requires(is_point()) : Matrix() {
    data[0][0] = other;
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const float (&arr)[ROWS]) noexcept requires(is_col()) : Matrix() {
    memcpy(this->data, arr, ROWS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const std::array<float, ROWS> &arr) noexcept requires(is_col()) : Matrix() {
    memcpy(this->data, arr.data(), COLS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator*(const Matrix &other) const noexcept requires(is_col()) {
    Matrix<1, 1> result;
    Matrix<1, ROWS> tmp = this->transpose();
    pl::matrix_mul(&tmp.matrix, &other.matrix, &result.matrix);
    return result[0];
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float &Matrix<ROWS, COLS, pl>::operator[](uint32_t num) noexcept requires(is_col()) { return this->data[num][0]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator[](uint32_t num) const noexcept requires(is_col()) { return this->data[num][0]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float &Matrix<ROWS, COLS, pl>::operator()(uint32_t num) noexcept requires(is_col()) { return this->data[num][0]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator()(uint32_t num) const noexcept requires(is_col()) { return this->data[num][0]; }


template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const float (&arr)[COLS]) noexcept requires(is_row()) : Matrix() {
    memcpy(this->data, arr, COLS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr Matrix<ROWS, COLS, pl>::Matrix(const std::array<float, COLS> &arr) noexcept requires(is_row()) : Matrix() {
    memcpy(this->data, arr.data(), COLS * sizeof(float));
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator*(const Matrix &other) const noexcept requires(is_row()) {
    Matrix<1, 1> result;
    Matrix<COLS, 1> tmp = other.transpose();
    pl::matrix_mul(&matrix, &tmp.matrix, &result.matrix);
    return result(1, 1);
}

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float &Matrix<ROWS, COLS, pl>::operator[](uint32_t num) noexcept requires(is_row() || is_point()) { return this->data[0][num]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator[](uint32_t num) const noexcept requires(is_row() || is_point()) { return this->data[0][num]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
float &Matrix<ROWS, COLS, pl>::operator()(uint32_t num) noexcept requires(is_row() || is_point()) { return this->data[0][num]; }

template<uint32_t ROWS, uint32_t COLS, mathPl pl>
constexpr float Matrix<ROWS, COLS, pl>::operator()(uint32_t num) const noexcept requires(is_row() || is_point()) { return this->data[0][num]; }

}
