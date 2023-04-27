/*
 * Battery.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include "Battery/Battery.hpp"

Battery::Battery(voltage_register_group* voltage_register1, voltage_register_group* voltage_register2, float* temp1, float* temp2) {
	cells[0] = &voltage_register1->voltage1;
	cells[1] = &voltage_register1->voltage2;
	cells[2] = &voltage_register1->voltage3;
	cells[3] = &voltage_register2->voltage1;
	cells[4] = &voltage_register2->voltage2;
	cells[5] = &voltage_register2->voltage3;
	temperature1 = temp1;
	temperature2 = temp2;
	minimum_cell = cells[0];
	maximum_cell = cells[0];
}

bool Battery::needs_balance() {
	for (float* cell : cells) {
		if (*cell > *maximum_cell) maximum_cell = cell;
		if (*cell < *minimum_cell) minimum_cell = cell;
	}

	if (SOC::calculate(*maximum_cell) - SOC::calculate(*minimum_cell) > SOC::MAX_DIFFERENCE) {
		return true;
	}

	return false;
}

float Battery::get_soc() {
	float min_value = 100;
	for (float* cell : cells) {
		float cell_soc = SOC::calculate(*cell);
		if (cell_soc < min_value) min_value = cell_soc;
	}

	return min_value;
}

float Battery::get_charging_maximum_voltage() {
	//TODO: change temperature units in class.
	uint16_t average_temperature = ((uint32_t) * temperature1 + * temperature2) / 2;
	
	if (average_temperature < 25) {
		return 4.05;
	}
	else if (average_temperature < 40) {
		return 4.1;
	}
	else {
		return 4.2;
	}
}
