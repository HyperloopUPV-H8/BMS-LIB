/*
 * LTC6811.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "Battery/Battery.hpp"
#include "PEC15/PEC15.hpp"

class LTC6811 {
public:
	static const uint8_t REGISTER_GROUPS = 4;
	static const uint8_t DATA_REGISTER_LENGTH = 6;
	static const uint8_t COMMAND_LENGTH = 2;
	static const uint8_t COMMAND_DATA_LENGTH = DATA_REGISTER_LENGTH + PEC15::LENGTH;

	LTC6811();

	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures[2];
	Battery battery1;
	Battery battery2;
};
