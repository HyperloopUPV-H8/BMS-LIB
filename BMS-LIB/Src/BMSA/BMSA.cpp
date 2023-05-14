/*
 * BMSA.cpp
 *
 *  Created on: 16 ene. 2023
 *      Author: aleja
 */

#include "BMSA/BMSA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

BMSA::BMSA(SPI::Peripheral& spi_peripheral) {
	optional<uint8_t> spi_optional = SPI::inscribe(spi_peripheral);

	if (not spi_optional) {
		ErrorHandler("SPI peripheral of BMSH did not register correctly");
		return;
	}

	spi_instance = spi_optional.value();
	external_adc = LTC6810();
}

span<BMSA::COMMAND> BMSA::get_cell_voltage_registers() {
	return span(cell_voltage_registers.begin(), cell_voltage_registers.end()-2);
}

void BMSA::copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number) {
	for (int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		external_adc.cell_voltages[voltage_number] = voltages[adc_number];
	}
}

