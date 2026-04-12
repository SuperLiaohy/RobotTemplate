#pragma once

#include "../Matrix/Matrix.hpp"

namespace EP::Math {

/**
 * @brief Extended Kalman Filter (EKF) base class using Curiously Recurring Template Pattern (CRTP).
 * 
 * @tparam Derived      The derived class implementing the system models (f, h, F, H, Q, R).
 * @tparam STATE_DIM    Dimension of the state vector.
 * @tparam Z_DIM        Dimension of the measurement vector.
 * @tparam U_DIM        Dimension of the control vector.
 */
template <typename Derived, uint32_t STATE_DIM, uint32_t Z_DIM, uint32_t U_DIM>
class Ekf {
public:
    using StateVec = ColVec<STATE_DIM>;
    using MeasVec  = ColVec<Z_DIM>;
    using CtrlVec  = ColVec<U_DIM>;
    using StateMat = Matrix<STATE_DIM, STATE_DIM>;
    using MeasMat  = Matrix<Z_DIM, Z_DIM>;
    using MeasToStateMat = Matrix<STATE_DIM, Z_DIM>;
    using StateToMeasMat = Matrix<Z_DIM, STATE_DIM>;

protected:
    StateVec x;
    StateMat P;

public:
    constexpr Ekf() noexcept : x(StateVec::zeros()), P(StateMat::eyes()) {}

    /** @brief Get the derived class instance */
    [[nodiscard]] constexpr Derived& derived() noexcept { return *static_cast<Derived*>(this); }
    [[nodiscard]] constexpr const Derived& derived() const noexcept { return *static_cast<const Derived*>(this); }

    /**
     * @brief Predict step of the EKF.
     * @param u Control vector.
     * @param dt Time step.
     */
    void predict(const CtrlVec& u, float dt) noexcept {
        StateMat F_mat = derived().F(x, u, dt);
        StateVec next_x = derived().f(x, u, dt);
        StateMat Q_mat = derived().Q();

        P = F_mat * P * (~F_mat) + Q_mat;
        x = next_x;
    }

    /**
     * @brief Update step of the EKF.
     * @param z Measurement vector.
     * @return true if the update was successful, false if the innovation covariance inversion failed.
     */
    [[nodiscard]] bool update(const MeasVec& z) noexcept {
        StateToMeasMat H_mat = derived().H(x);
        MeasVec h_vec = derived().h(x);
        MeasMat R_mat = derived().R();

        MeasVec y = z - h_vec;
        MeasMat S = H_mat * P * (~H_mat) + R_mat;

        MeasMat S_inv;
        auto status = S.inv(S_inv);
        if (status != MeasMat::BackendPl::SUCCESS) {
            return false;
        }

        MeasToStateMat K = P * (~H_mat) * S_inv;
        x = x + K * y;
        
        StateMat I = StateMat::eyes();
        P = (I - K * H_mat) * P;

        return true;
    }

    /** @brief Get the current state vector */
    [[nodiscard]] constexpr const StateVec& getState() const noexcept { return x; }

    /** @brief Set the state vector */
    constexpr void setState(const StateVec& new_x) noexcept { x = new_x; }

    /** @brief Get the current covariance matrix */
    [[nodiscard]] constexpr const StateMat& getCovariance() const noexcept { return P; }

    /** @brief Set the covariance matrix */
    constexpr void setCovariance(const StateMat& new_P) noexcept { P = new_P; }
};

} // namespace EP::Math
