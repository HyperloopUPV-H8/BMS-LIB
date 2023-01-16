/*
 * LTC681X.cpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#include "LTC681X/LTC681X.hpp"

LTC681X::configuration::configuration() {
	// default config
	for (uint8_t cell : iota(0, 5)) set_gpio(cell, true);
	set_references(true);
	set_adc_optimal_mode(true);
	set_discharge_timer(true);
	for (uint8_t cell : iota(0, 12)) set_cell_discharging(cell, false);
	set_discharge_timeout_value(THIRTY_MINUTES);
	set_adc_undervoltage(3.3);
	set_adc_overvoltage(4.2);
}

void LTC681X::configuration::set_gpio(uint8_t gpio, bool state) {
	if (gpio < 0 or gpio >= 5 ) return; //TODO: Error handler
	register_group[0][3+gpio] = state;
}

void LTC681X::configuration::set_references(bool state) {
	register_group[0][2] = state;
}

void LTC681X::configuration::set_adc_optimal_mode(bool state) {
	register_group[0][0] = state;
}

void LTC681X::configuration::set_discharge_timer(bool state) {
	register_group[0][1] = state;
}

void LTC681X::configuration::set_cell_discharging(uint8_t cell, bool state) {
	if (cell < 8) register_group[4][cell] = state;
	if (cell < 12) register_group[5][cell-8] = state;
	else return; //TODO: Error handler
}

void LTC681X::configuration::set_discharge_timeout_value(DISCHARGE_TIME discharge_time) {
	bitset<4> discharge_time_bits = (bitset<4>)discharge_time;
	for (uint8_t i : iota(0, 4)) {
		register_group[5][i+4] = discharge_time_bits[i];
	}
}

void LTC681X::configuration::set_adc_undervoltage(float adc_voltage) {
	bitset<12> undervoltage_bits = ((uint16_t)(adc_voltage * 10000) >> 4) - 1;
	for (uint8_t i : iota(0, 8)) {
		register_group[1][i] = undervoltage_bits[i];
	}
	for (uint8_t i : iota(0, 4)) {
		register_group[2][i] = undervoltage_bits[i+8];
	}
}

void LTC681X::configuration::set_adc_overvoltage(float adc_voltage) {
	bitset<12> overvoltage_bits = (uint16_t)(adc_voltage * 10000) >> 4;
	for(uint8_t i : iota(0, 4)) {
		register_group[2][i+4] = overvoltage_bits[i];
	}
	for(uint8_t i : iota(0, 8)) {
		register_group[3][i] = overvoltage_bits[i+4];
	}
}

void set_adc_overvoltage(float adc_voltage) {

}
bool LTC681X::configuration::is_gpio_on(uint8_t gpio) {
	if (gpio < 0 or gpio >= 5 ) return 0; //TODO: Error handler
	return register_group[0][3+gpio];
}
bool LTC681X::configuration::is_references_on() { return register_group[0][2];}
bool LTC681X::configuration::is_discharge_timer_enabled() { return register_group[0][1];}
bool LTC681X::configuration::is_adc_optimal_mode() {return register_group[0][0];}
bool LTC681X::configuration::is_cell_discharging(uint8_t cell) {
	if (cell < 8) return register_group[4][cell];
	else if (cell < 12) return register_group[5][cell-8];
	else return 0; //TODO: Error handler
}
uint8_t LTC681X::configuration::get_discharge_timeout_value() {
	uint8_t timeout_value = 0;
	for (bool i : iota(0,4)) {
		timeout_value += register_group[5][i+4] << i;
	}
	return timeout_value;
}

float LTC681X::configuration::get_adc_undervoltage() {
	uint16_t undervoltage_value = 0;
	for (uint8_t i : iota(0, 8)) {
		undervoltage_value += (uint16_t)register_group[1][i] << i;
	}
	for (uint8_t i : iota (0,4)) {
		undervoltage_value += (uint16_t)register_group[2][i] << (i+8);
	}
	return ((undervoltage_value+1) << 4) * 0.0001;
}

float LTC681X::configuration::get_adc_overvoltage() {
	uint16_t overvoltage_value = 0;
	for (uint8_t i : iota(0, 4)) {
		overvoltage_value += (uint16_t)register_group[2][i+4] << i;
	}
	for (uint8_t i: iota(0, 8)) {
		overvoltage_value += (uint16_t)register_group[3][i] << (i+4);
	}

	return (overvoltage_value << 4) * 0.0001;
}
