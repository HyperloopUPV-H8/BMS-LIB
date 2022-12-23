/*
 * LTC6811.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#define REGISTER_GROUPS 4

#include "ST-LIB.hpp"
#include "Battery/Battery.hpp"

class LTC6811 {
public:
	LTC6811();

	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures[2];
	Battery battery1;
	Battery battery2;
};
