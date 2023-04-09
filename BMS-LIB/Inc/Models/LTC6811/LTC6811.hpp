/*
 * LTC6811.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#include "LTC681X/LTC681X.hpp"
#include "Battery/Battery.hpp"

class LTC6811 : public LTC681X {
public:
	static const uint8_t REGISTER_GROUPS = 4;

	LTC6811();

	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures[2];
	Battery batteries[2];

	float internal_temperature;
};
