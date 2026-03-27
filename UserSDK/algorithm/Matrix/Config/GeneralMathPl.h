//
// Created by liaohy on 3/17/25.
//
#pragma once

#include <cmath>
#include <cstring>

namespace EP::Math {
using float32_t = float;
using float64_t = double;

class GeneralMathPl {
public:
    enum status {
        SUCCESS = 0, /**< No error */
        ARGUMENT_ERROR = -1, /**< One or more arguments are incorrect */
        LENGTH_ERROR = -2, /**< Length of data buffer is incorrect */
        SIZE_MISMATCH = -3, /**< Size of matrices is not compatible with the operation */
        NANINF = -4, /**< Not-a-number (NaN) or infinity is generated */
        SINGULAR = -5, /**< Input matrix is singular and cannot be inverted */
        TEST_FAILURE = -6, /**< Test Failed */
        DECOMPOSITION_FAILURE = -7 /**< Decomposition Failed */
    };

    struct MatrixView {
        uint16_t numRows; /**< number of rows of the matrix.     */
        uint16_t numCols; /**< number of columns of the matrix.  */
        float32_t *pData; /**< points to the data of the matrix. */
    };

    GeneralMathPl() = default;

    [[gnu::always_inline]] inline static void
    matrix_init(MatrixView *self, uint16_t rows, uint16_t cols, float *data) {
        self->numRows = rows;
        self->numCols = cols;
        self->pData = data;
    };

    [[gnu::always_inline]] inline static void
    matrix_add(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        result->numCols = self->numCols;
        result->numRows = self->numRows;
        for (int i = 0; i < self->numRows; ++i) {
            for (int j = 0; j < self->numCols; ++j) {
                result->pData[i * self->numCols + j] =
                        self->pData[i * self->numCols + j] + other->pData[i * self->numCols + j];
            }
        }
    };

    [[gnu::always_inline]] inline static void
    matrix_sub(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        result->numCols = self->numCols;
        result->numRows = self->numRows;
        for (int i = 0; i < self->numRows; ++i) {
            for (int j = 0; j < self->numCols; ++j) {
                result->pData[i * self->numCols + j] =
                        self->pData[i * self->numCols + j] - other->pData[i * self->numCols + j];
            }
        }
    };

    [[gnu::always_inline]] inline static void
    matrix_mul(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        result->numRows = self->numRows;
        result->numCols = other->numCols;
        for (int i = 0; i < result->numRows; ++i) {
            for (int j = 0; j < result->numCols; ++j) {
                float sum = 0;
                for (int k = 0; k < self->numCols; ++k) {
                    sum = sum + self->pData[i * self->numCols + k] * other->pData[k * other->numCols + j];
                }
                result->pData[i * result->numCols + j] = sum;
            }
        }
    };

    [[gnu::always_inline]] inline static void
    matrix_scale(const MatrixView *self, float scale, MatrixView *result) {
        result->numRows = self->numRows;
        result->numCols = self->numCols;
        for (int i = 0; i < self->numRows; ++i) {
            for (int j = 0; j < self->numCols; ++j) {
                result->pData[i * self->numCols + j] = self->pData[i * self->numCols + j] * scale;
            }
        }
    };

    [[gnu::always_inline]] inline static void
    matrix_trans(const MatrixView *self, MatrixView *result) {
        result->numRows = self->numCols;
        result->numCols = self->numRows;
        for (int i = 0; i < result->numRows; ++i) {
            for (int j = 0; j < result->numCols; ++j) {
                result->pData[i * result->numCols + j] = self->pData[j * self->numCols + i];
            }
        }
    };

    static status
    matrix_inverse(MatrixView *self, MatrixView *result) {
        MatrixView selfView;
        MatrixView selfLastView;
        MatrixView resultView;
        MatrixView resultLastView;

        memset(result->pData, 0, result->numRows * result->numCols * sizeof(float32_t));
        for (int i = 0; i < result->numRows; ++i) {
            for (int j = 0; j < result->numCols; ++j) {
                if (i == j) {
                    result->pData[i * result->numCols + j] = 1;
                }
            }
        }
        matrix_init(&selfView, 1, self->numCols, self->pData);
        matrix_init(&selfLastView, 1, self->numCols, self->pData);
        matrix_init(&resultView, 1, result->numCols, result->pData);
        matrix_init(&resultLastView, 1, result->numCols, result->pData);

        float scale = self->pData[0];

        for (int i = 0; i < self->numRows; ++i) {
            resultView.pData = &result->pData[i * result->numCols];
            selfView.pData = &self->pData[i * self->numCols];
            int index;
            for (index = 0; index < self->numCols; ++index) {
                if (std::abs(selfView.pData[index]) > 1e-6f) break;
            }
            if (index == self->numCols) { return SINGULAR; }
            for (int j = 0; j < self->numRows; ++j) {
                if (j == i) continue;
                selfLastView.pData = &self->pData[j * self->numCols];
                resultLastView.pData = &result->pData[j * result->numCols];

                if (std::abs(selfLastView.pData[index]) <= 1e-6f) continue;
                scale = selfLastView.pData[index] / selfView.pData[index];
                matrix_scale(&selfView, scale, &selfView);
                matrix_scale(&resultView, scale, &resultView);

                matrix_sub(&selfLastView, &selfView, &selfLastView);
                matrix_sub(&resultLastView, &resultView, &resultLastView);
            }
            scale = 1 / selfView.pData[index];
            matrix_scale(&selfView, scale, &selfView);
            matrix_scale(&resultView, scale, &resultView);
        }
        for (int i = 0; i < self->numRows; ++i) {
            selfView.pData = &self->pData[i * self->numCols];
            resultView.pData = &result->pData[i * result->numCols];
            if (selfView.pData[i] > 1e-6f) continue;
            for (int j = i + 1; j < self->numRows; ++j) {
                selfLastView.pData = &self->pData[j * self->numCols];
                resultLastView.pData = &result->pData[j * result->numCols];
                if (selfLastView.pData[i] > 1e-6f) {
                    memswap_xor(selfLastView.pData, selfView.pData, selfView.numCols * sizeof(float32_t));
                    memswap_xor(resultLastView.pData, resultView.pData, resultView.numCols * sizeof(float32_t));
                }
            }
        }
        return SUCCESS;
    }

    [[gnu::always_inline]] inline static float
    sin(float input) {
        return std::sin(input);
    }

    [[gnu::always_inline]] inline static float
    cos(float input) {
        return std::cos(input);
    }

    [[gnu::always_inline]] inline static float
    sqrtf(float input) {
        return std::sqrt(input);
    }

    [[gnu::always_inline]] inline static float
    acos(float input) {
        return std::acos(input);
    }

    [[gnu::always_inline]] inline static float
    asin(float input) {
        return std::asin(input);
    }

    [[gnu::always_inline]] inline static float
    atan2(float y, float x) {
        return std::atan2(y,x);
    }



private:
    static void memswap_xor(void *pa, void *pb, size_t n) {
        char *a = (char *) pa;
        char *b = (char *) pb;
        for (size_t i = 0; i < n; ++i) {
            a[i] ^= b[i];
            b[i] ^= a[i];
            a[i] ^= b[i];
        }
    }
};
}
