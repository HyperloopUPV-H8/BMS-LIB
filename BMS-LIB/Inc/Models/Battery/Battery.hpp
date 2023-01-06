/*
 * Battery.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"

struct voltage_register_group {
	uint16_t voltage1;
	uint16_t voltage2;
	uint16_t voltage3;
};

class Battery {
public:
	static const uint8_t CELLS = 6;
	static const uint8_t MAX_SOC_DIFFERENCE = 100;
	static const uint8_t MIN_CELL_VOLTAGE = 3.4;
	static const uint8_t MAX_CELL_VOLTAGE = 4.2;

	uint16_t cells[CELLS];
	uint16_t temperature1;
	uint16_t temperature2;

	Battery() = default;
	Battery(voltage_register_group& cell_register1, voltage_register_group& cell_register2, uint16_t& temperature1, uint16_t& temperature2);

	bool needs_balance();
	uint16_t get_soc();

private:
	uint16_t get_cell_soc(uint16_t cell);
	float get_charging_maximum_voltage();

	static uint16_t soc[901];
};
