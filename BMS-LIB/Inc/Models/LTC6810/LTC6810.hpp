/*
 * LTC6810.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "Battery/Battery.hpp"

class LTC6810 : public LTC681X {
public:
	static const uint8_t REGISTER_GROUPS = 2;

	LTC6810() = default;
	voltage_register_group cell_voltages[REGISTER_GROUPS];
};
