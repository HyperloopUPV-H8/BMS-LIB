/*
 * BMSA.cpp
 *
 *  Created on: 16 ene. 2023
 *      Author: aleja
 */

#include "BMSA/BMSA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

BMSA::BMSA(uint8_t spi_instance) {
	this->spi_instance = spi_instance;
}

span<BMSA::COMMAND> BMSA::get_cell_voltage_registers() {
	return span(cell_voltage_registers.begin(), cell_voltage_registers.end());
}

void BMSA::copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number) {
	for (int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		external_adc.cell_voltages[voltage_number] = voltages[adc_number];
	}
}

