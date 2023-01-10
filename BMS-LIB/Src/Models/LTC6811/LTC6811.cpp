/*
 * LTC681x.cpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#include <LTC6811/LTC6811.hpp>

bool LTC6811::configuration::is_gpio_on(uint8_t gpio) {
	if (gpio < 0 or gpio >= 5 ) return 0; //TODO: Error handler
	return register_group[0][3+gpio];
}
bool LTC6811::configuration::is_references_on() { return register_group[0][2];}
bool LTC6811::configuration::is_discharge_timer_enabled() { return register_group[0][1];}
bool LTC6811::configuration::is_adc_optimal_mode() {return register_group[0][0];}
bool LTC6811::configuration::is_cell_discharging(uint8_t cell) {
	if (cell < 8) return register_group[4][cell];
	else if (cell < 12) return register_group[5][cell-8];
	else return 0; //TODO: Error handler
}
uint8_t LTC6811::configuration::get_discharge_timeout_value() {
	uint8_t timeout_value = 0;
	for (bool i : iota(0,4)) {
		timeout_value += register_group[5][i+4] << i;
	}
	return timeout_value;
}

float LTC6811::configuration::get_adc_undervoltage_comparison() {
	uint16_t undervoltage_value = 0;
	for (uint8_t i : iota(0, 8)) {
		undervoltage_value += (uint16_t)register_group[1][i] << i;
	}
	for (uint8_t i : iota (0,4)) {
		undervoltage_value += (uint16_t)register_group[2][i] << i+8;
	}
	return ((undervoltage_value+1) << 4) * 0.0001;
}

float LTC6811::configuration::get_adc_overvoltage_comparison() {
	uint16_t overvoltage_value = 0;
	for (uint8_t i : iota(0, 4)) {
		overvoltage_value += (uint16_t)register_group[2][i+4] << i;
	}
	for (uint8_t i: iota(0, 8)) {
		overvoltage_value += (uint16_t)register_group[3][i] << i+4;
	}

	return (overvoltage_value << 4) * 0.0001;
}


LTC6811::LTC6811() : cell_voltages{ 0 }, temperatures{ 0 } {
	battery1 = Battery(&cell_voltages[0], &cell_voltages[1], &temperatures[0].voltage1, &temperatures[0].voltage2);
	battery2 = Battery(&cell_voltages[2], &cell_voltages[3], &temperatures[0].voltage3, &temperatures[1].voltage1);
	peripheral_configuration = configuration();
}
