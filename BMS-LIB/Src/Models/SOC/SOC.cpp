/*
 * SOC.cpp
 *
 *  Created on: 6 feb. 2023
 *      Author: aleja
 */

#include "SOC/SOC.hpp"

uint16_t SOC::calculate(uint16_t raw_voltage) {
	float real_voltage = raw_voltage / 10000.0;

	//Cambiar por Protection Manager
	if (real_voltage > Cell::MAX_VOLTAGE or real_voltage < Cell::MIN_VOLTAGE) {
		//TODO: Fault Handler (Es una protección)
		return 0;
	}

	uint16_t soc_index = (real_voltage - Cell::MIN_VOLTAGE) * 1000;
	return voltage_to_soc[soc_index];
}
