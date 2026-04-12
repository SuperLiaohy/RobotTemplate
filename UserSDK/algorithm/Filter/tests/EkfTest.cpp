#include <gtest/gtest.h>
#include "Filter/Ekf.hpp"

using namespace EP::Math;

class TestEkf : public Ekf<TestEkf, 2, 1, 1> {
public:
    StateVec f(const StateVec& x, const CtrlVec& u, float dt) const noexcept {
        StateVec next_x = StateVec::zeros();
        next_x(0, 0) = x(0, 0) + x(1, 0) * dt + 0.5f * u(0, 0) * dt * dt;
        next_x(1, 0) = x(1, 0) + u(0, 0) * dt;
        return next_x;
    }

    StateMat F(const StateVec&, const CtrlVec&, float dt) const noexcept {
        StateMat mat = StateMat::eyes();
        mat(0, 1) = dt;
        return mat;
    }

    StateMat Q() const noexcept {
        StateMat mat = StateMat::zeros();
        mat(0, 0) = 0.1f;
        mat(1, 1) = 0.1f;
        return mat;
    }

    MeasVec h(const StateVec& x) const noexcept {
        MeasVec z = MeasVec::zeros();
        z(0, 0) = x(0, 0);
        return z;
    }

    StateToMeasMat H(const StateVec&) const noexcept {
        StateToMeasMat mat = StateToMeasMat::zeros();
        mat(0, 0) = 1.0f;
        mat(0, 1) = 0.0f;
        return mat;
    }

    MeasMat R() const noexcept {
        MeasMat mat = MeasMat::zeros();
        mat(0, 0) = 0.5f;
        return mat;
    }
};

TEST(EkfTest, PredictStateAndCovariance) {
    TestEkf ekf;
    TestEkf::CtrlVec u = TestEkf::CtrlVec::zeros();
    u(0, 0) = 2.0f;
    
    ekf.predict(u, 0.5f);
    
    EXPECT_NEAR(ekf.getState()(0, 0), 0.25f, 1e-5f);
    EXPECT_NEAR(ekf.getState()(1, 0), 1.0f, 1e-5f);
    
    EXPECT_NEAR(ekf.getCovariance()(0, 0), 1.35f, 1e-5f);
    EXPECT_NEAR(ekf.getCovariance()(0, 1), 0.5f, 1e-5f);
    EXPECT_NEAR(ekf.getCovariance()(1, 0), 0.5f, 1e-5f);
    EXPECT_NEAR(ekf.getCovariance()(1, 1), 1.1f, 1e-5f);
}

TEST(EkfTest, UpdateCorrectionDirection) {
    TestEkf ekf;
    
    TestEkf::StateVec x = TestEkf::StateVec::zeros();
    x(0, 0) = 1.0f;
    x(1, 0) = 0.0f;
    ekf.setState(x);
    
    TestEkf::CtrlVec u = TestEkf::CtrlVec::zeros();
    u(0, 0) = 0.0f;
    ekf.predict(u, 1.0f);
    
    float pos_pred = ekf.getState()(0, 0);
    
    TestEkf::MeasVec z = TestEkf::MeasVec::zeros();
    z(0, 0) = 3.0f;
    
    bool status = ekf.update(z);
    EXPECT_TRUE(status);
    
    EXPECT_GT(ekf.getState()(0, 0), pos_pred);
}

class SingularEkf : public Ekf<SingularEkf, 2, 1, 1> {
public:
    StateVec f(const StateVec&, const CtrlVec&, float) const noexcept { return StateVec::zeros(); }
    StateMat F(const StateVec&, const CtrlVec&, float) const noexcept { return StateMat::eyes(); }
    StateMat Q() const noexcept { return StateMat::zeros(); }
    MeasVec h(const StateVec&) const noexcept { return MeasVec::zeros(); }
    StateToMeasMat H(const StateVec&) const noexcept { return StateToMeasMat::zeros(); }
    
    MeasMat R() const noexcept {
        return MeasMat::zeros();
    }
};

TEST(EkfTest, SingularInnovation) {
    SingularEkf bad_ekf;
    bad_ekf.setCovariance(SingularEkf::StateMat::zeros());
    
    SingularEkf::MeasVec z = SingularEkf::MeasVec::zeros();
    z(0, 0) = 1.0f;
    
    bool status = bad_ekf.update(z);
    EXPECT_FALSE(status);
}
