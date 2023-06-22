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

ChargingControl::ChargingControl(double* input_current, bool* want_to_charge, float* SOC, Pin& positive_pwm_pin, Pin& positive_pwm_negated_pin,
		Pin& negative_pwm_pin, Pin& negative_pwm_negated_pin, Pin& enable_pin) {
    state_machine = StateMachine(States::PRECHARGE);
    dclv = HalfBridge(positive_pwm_pin, positive_pwm_negated_pin, negative_pwm_pin, negative_pwm_negated_pin, enable_pin);

    charger_pi = PI<IntegratorType::Trapezoidal>(-20, -1, 0.0002);
    this->input_charging_current = input_current;
    this->want_to_charge = want_to_charge;
    this->SOC = SOC;
}

void ChargingControl::start() {

    state_machine.add_state(States::CONSTANT_CURRENT);
    state_machine.add_state(States::CONSTANT_VOLTAGE);

    state_machine.add_transition(States::PRECHARGE, States::CONSTANT_CURRENT, [&]() {
        return dclv.get_phase() <= 4;
    });

    state_machine.add_transition(States::CONSTANT_CURRENT, States::CONSTANT_VOLTAGE, [&]() {
        if (*SOC >= 80) {
            return true;
        }

        return false;
    });

    state_machine.add_transition(States::CONSTANT_VOLTAGE, States::CONSTANT_CURRENT, [&]() {
        if (*SOC <= 60) {
            return true;
        }
    });

    state_machine.add_enter_action([&]() {
        pwm_frequency = 100000;
        dclv.set_phase(100);
        dclv.set_duty_cycle(50);
        dclv.set_frequency(200000);
        charger_pi.reset();
        dclv.turn_on();
    }, States::PRECHARGE);

    state_machine.add_low_precision_cyclic_action([&]() {
        if (*input_charging_current < 1) {
            *want_to_charge = false;
        }
    }, ms(100), States::CONSTANT_VOLTAGE);

    state_machine.add_low_precision_cyclic_action([&]() {
        if (dclv.get_phase() > 3) {
            dclv.set_phase(dclv.get_phase() - 1);
        }
    }, ms(50), States::PRECHARGE);

    state_machine.add_exit_action( [&]() {
        dclv.set_phase(3);
    }, States::PRECHARGE);
}
