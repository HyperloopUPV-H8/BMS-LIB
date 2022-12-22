/*
 * LTC6811.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#define REGISTER_GROUPS 4

#define MINIMUM_CELL_VOLTAGE 3.6 // Ask Hardware
#define MAXIMUM_CELL_VOLTAGE 4.2

#include "ST-LIB.hpp"

struct voltage_register_group {
	uint16_t voltage1;
	uint16_t voltage2;
	uint16_t voltage3;
};
struct battery {
	uint16_t cell1;
	uint16_t cell2;
	uint16_t cell3;
	uint16_t cell4;
	uint16_t cell5;
	uint16_t cell6;
	uint16_t temperature1;
	uint16_t temperature2;

	battery() = default;
	battery(voltage_register_group& cell_register1, voltage_register_group& cell_register2, uint16_t& temperature1, uint16_t& temperature2);
};

class LTC6811 {
public:
	LTC6811();

	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures[2];
	battery battery1;
	battery battery2;
};
