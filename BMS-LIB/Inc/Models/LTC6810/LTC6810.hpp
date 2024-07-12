/*
 * LTC6810.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "LTC681X/LTC681X.hpp"
#include "Battery/Battery.hpp"

class LTC6810 : public LTC681X {
public:
	static const uint8_t REGISTER_GROUPS = 2;

	void initialize();

	LTC6810();
	voltage_register_group cell_voltages[REGISTER_GROUPS];
	voltage_register_group temperatures1;
	voltage_register_group temperatures2;
	Battery battery;
	bool is_battery_connected()const{
		return battery.is_adc_connected();
	}
	float internal_temperature;
};
