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
    using status = arm_status;
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
        return arm_mat_inverse_f32(self, result);
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
};
}
#endif
