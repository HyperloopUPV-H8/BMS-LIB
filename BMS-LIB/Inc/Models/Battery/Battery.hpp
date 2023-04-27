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
	float voltage1;
	float voltage2;
	float voltage3;
};

class Battery {
public:
	static const int CELLS = 6;
	static const int MIN_TEMPERATURE = 0;
	static const int MAX_TEMPERATURE = 60;

	float* cells[CELLS];
	float* minimum_cell;
	float* maximum_cell;

	float* temperature1;
	float* temperature2;

	Battery() = default;
	Battery(voltage_register_group* cell_register1, voltage_register_group* cell_register2, float* temperature1, float* temperature2);

	bool needs_balance();
	float get_soc();

private:
	float get_charging_maximum_voltage();

	static const uint16_t soc[901];
};
