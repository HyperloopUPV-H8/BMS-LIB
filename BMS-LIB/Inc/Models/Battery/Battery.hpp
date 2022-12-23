/*
 * Battery.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#define CELLS 6
#define MAXIMUM_DIFERENCE 0.01
#define MINIMUM_CELL_VOLTAGE 3.273
#define MAXIMUM_CELL_VOLTAGE 4.2

#include "ST-LIB.hpp"

struct voltage_register_group {
	uint16_t voltage1;
	uint16_t voltage2;
	uint16_t voltage3;
};

class Battery {
public:
	uint16_t cells[CELLS];
	uint16_t temperature1;
	uint16_t temperature2;

	Battery() = default;
	Battery(voltage_register_group& cell_register1, voltage_register_group& cell_register2, uint16_t& temperature1, uint16_t& temperature2);

	uint8_t balancing_algorithm();
	uint8_t calculate_soc(uint16_t cell);

	static uint16_t soc[901];
};
