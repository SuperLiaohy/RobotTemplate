//
// Created by liaohy on 3/17/25.
//
#pragma once

#include "concepts"
#include "cstdint"
#include "GeneralMathPl.h"
#include "ArmMathPl.h"

namespace EP::Math {
template<typename T>
concept mathPl = requires(T t) {
    typename T::status;
    typename T::MatrixView;
    std::declval<typename T::MatrixView>().pData;

    T::matrix_init(std::declval<typename T::MatrixView *>(),
                   uint16_t(), uint16_t(), std::declval<float *>());

    T::matrix_add(std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>());

    T::matrix_sub(std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>());

    T::matrix_mul(std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>(),
                  std::declval<typename T::MatrixView *>());

    T::matrix_scale(std::declval<typename T::MatrixView *>(),
                    std::declval<float>(),
                    std::declval<typename T::MatrixView *>());

    T::matrix_trans(std::declval<typename T::MatrixView *>(),
                    std::declval<typename T::MatrixView *>());

    T::matrix_inverse(std::declval<typename T::MatrixView *>(),
                      std::declval<typename T::MatrixView *>());
};
#ifdef USE_HAL_DRIVER
class DefaultMathPl : public ArmMathPl {
};
#else
class DefaultMathPl : public GeneralMathPl {
};
#endif

}
