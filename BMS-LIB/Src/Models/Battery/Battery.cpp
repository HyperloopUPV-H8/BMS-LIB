/*
 * Battery.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include "Battery/Battery.hpp"

Battery::Battery(voltage_register_group& voltage_register1, voltage_register_group& voltage_register2, uint16_t& temp1, uint16_t& temp2) {
	cells[0] = voltage_register1.voltage1;
	cells[1] = voltage_register1.voltage2;
	cells[2] = voltage_register1.voltage3;
	cells[3] = voltage_register2.voltage1;
	cells[4] = voltage_register2.voltage2;
	cells[5] = voltage_register2.voltage3;
	temperature1 = temp1;
	temperature2 = temp2;
}

uint8_t Battery::balancing_algorithm() {
	uint16_t max_value = cells[0];
	uint16_t min_value = cells[0];

	for (uint8_t cell_number = 1; cell_number < CELLS; cell_number++) {
		if (cells[cell_number] > max_value) {
			max_value = cells[cell_number];
		}

		if (cells[cell_number] < min_value) {
			min_value = cells[cell_number];
		}

		float percentage_difference = ((float)max_value - min_value) / max_value;

		if (not percentage_difference > MAXIMUM_DIFERENCE) {
			return 0;
		}

		// balance
	}


}
