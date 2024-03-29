/*
 * LTC6810.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "Battery/Battery.hpp"
#include "LTC681X/LTC681X.hpp"

class LTC6810 : public LTC681X {
public:
	static const uint8_t REGISTER_GROUPS = 2;

	void initialize();

	LTC6810();
	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures;
	Battery battery;

	float internal_temperature;
};
