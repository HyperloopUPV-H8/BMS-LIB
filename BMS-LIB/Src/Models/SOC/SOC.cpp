/*
 * SOC.cpp
 *
 *  Created on: 6 feb. 2023
 *      Author: aleja
 */

#include "SOC/SOC.hpp"

float SOC::calculate(float real_voltage) {
	//Cambiar por Protection Manager
	if (real_voltage > Cell::MAX_VOLTAGE or real_voltage < Cell::MIN_VOLTAGE) {
		//TODO: Fault Handler (Es una protección)
		return 0;
	}

	uint16_t soc_index = (real_voltage - Cell::MIN_VOLTAGE) * 1000;
	return voltage_to_soc[soc_index] / 100.0;
}
