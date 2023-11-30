#pragma once

#include "ST-LIB.hpp"

class ChargingControl {
private:
    enum States {
        PRECHARGE = 100,
        CONSTANT_CURRENT = 101,
        CONSTANT_VOLTAGE = 102
    };

    double* input_charging_current;
    bool* want_to_charge;
    float* SOC;

public:
    double pwm_frequency = 100000;
    double control_frequency = 5000;

    PI<IntegratorType::Trapezoidal> charger_pi;
    HalfBridge dclv;

    ChargingControl() = default;
    ChargingControl(double* input_current, bool* want_to_charge, float* SOC, Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin);
    void start();

    StateMachine state_machine;
};


