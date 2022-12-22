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

struct Battery {
	uint16_t cell1;
	uint16_t cell2;
	uint16_t cell3;
	uint16_t cell4;
	uint16_t cell5;
	uint16_t cell6;
	uint16_t temperature1;
	uint16_t temperature2;

	Battery() = default;
	Battery(voltage_register_group& cell_register1, voltage_register_group& cell_register2, uint16_t& temperature1, uint16_t& temperature2);
};
