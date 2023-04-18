/*
 * Battery.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "SOC/SOC.hpp"

using std::views::iota;

struct voltage_register_group {
	uint16_t voltage1;
	uint16_t voltage2;
	uint16_t voltage3;
};

class Battery {
public:
	static const int CELLS = 6;
	static const int MAX_TEMPERATURE = 60;

	uint16_t* cells[CELLS];
	uint16_t* minimum_cell;
	uint16_t* maximum_cell;

	uint16_t* temperature1;
	uint16_t* temperature2;

	Battery() = default;
	Battery(voltage_register_group* cell_register1, voltage_register_group* cell_register2, uint16_t* temperature1, uint16_t* temperature2);

	bool needs_balance();
	uint16_t get_soc();

private:
	uint16_t get_cell_soc(uint16_t cell);
	float get_charging_maximum_voltage();

	static const uint16_t soc[901];
};
