//
// Created by liaohy on 12/6/25.
//

#pragma once
#include "Platform.h"
#include <cmath>
#include <cstring>

#ifdef USE_HAL_DRIVER
#ifdef __cplusplus
extern "C" {
#endif
#include "arm_math.h"
#ifdef __cplusplus
}
#endif

namespace EP::Math {
class ArmMathPl {
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
    using MatrixView = arm_matrix_instance_f32;

    [[gnu::always_inline]] inline static void
    matrix_init(MatrixView *self, uint16_t rows, uint16_t cols, float *data) {
        arm_mat_init_f32(self, rows, cols, data);
    };

    [[gnu::always_inline]] inline static void
    matrix_add(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        arm_mat_add_f32(self, other, result);
    };

    [[gnu::always_inline]] inline static void
    matrix_sub(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        arm_mat_sub_f32(self, other, result);
    };

    [[gnu::always_inline]] inline static void
    matrix_mul(const MatrixView *self, const MatrixView *other, MatrixView *result) {
        arm_mat_mult_f32(self, other, result);
    };

    [[gnu::always_inline]] inline static void
    matrix_scale(const MatrixView *self, float scale, MatrixView *result) {
        arm_mat_scale_f32(self, scale, result);
    };

    [[gnu::always_inline]] inline static void
    matrix_trans(const MatrixView *self, MatrixView *result) {
        arm_mat_trans_f32(self, result);
    };

    static status
    matrix_inverse(MatrixView *self, MatrixView *result) {
        return static_cast<status>(arm_mat_inverse_f32(self, result));
    }

    [[gnu::always_inline]] inline static float
    sin(float input) {
        return arm_sin_f32(input);
    }

    [[gnu::always_inline]] inline static float
    cos(float input) {
        return arm_cos_f32(input);
    }

    [[gnu::always_inline]] inline static float
    sqrtf(float input) {
        float result;
        arm_sqrt_f32(input, &result);
        return result;
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
        float result;
        arm_atan2_f32(y, x, &result);
        return result;
    }
};
}
#endif
