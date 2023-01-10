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
	enum DISCHARGE_TIME : uint8_t {
		DISABLED = 0,
		HALF_MINUTE = 1,
		ONE_MINUTE = 2,
		TWO_MINUTES = 3,
		THREE_MINUTES = 4,
		FOUR_MINUTES = 5,
		FIVE_MINUTES = 6,
		TEN_MINUTES = 7,
		FIFTEEN_MINUTES = 8,
		TWENTY_MINUTES = 9,
		THIRTY_MINUTES = 10,
		FORTY_MINUTES = 11,
		ONE_HOUR = 12,
		SEVENTY_FIVE_MINUTES = 13,
		NINETY_MINUTES = 14,
		TWO_HOURS = 15

	};

	static const uint8_t REGISTER_GROUPS = 4;
	static const uint8_t DATA_REGISTER_LENGTH = 6;
	static const uint8_t COMMAND_LENGTH = 2;
	static const uint8_t COMMAND_DATA_LENGTH = DATA_REGISTER_LENGTH + PEC15::LENGTH;

private:
	class configuration {
		array<PinState, 5> gpio;
		PinState references_on;
		PinState discharge_timer;
		PinState adc_optimal_mode;
		uint8_t undervoltage_comparison_voltage;
		uint8_t overvoltage_comparison_voltage;
		array<PinState, 12> discharge_cell;
		DISCHARGE_TIME discharge_timeout;
	};

	array<uint8_t, 48> configuration_register;
public:
	LTC6811();

	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures[2];
	Battery battery1;
	Battery battery2;
};
