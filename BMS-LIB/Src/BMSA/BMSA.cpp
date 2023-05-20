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

void BMSA::parse_configuration_data_stream(span<uint8_t> data_stream) {
	uint8_t offset = 0;
	for (bitset<8> data_register : external_adc.peripheral_configuration.register_group) {
		data_stream[offset] = (uint8_t)data_register.to_ulong(); //TODO: Refactor this, is horrible.
		offset++;
	}
}

void BMSA::check_adcs() {
	Battery battery = external_adc.battery;

	if(not battery.needs_balance()) {
		return;
	}

	for(uint8_t i : iota(0, (int)Battery::CELLS)) {
		float& min_cell = *battery.minimum_cell;
		float& curr_cell = *battery.cells[i];
		if (int(SOC::calculate(curr_cell)) - int(SOC::calculate(min_cell)) > SOC::MAX_DIFFERENCE) {
			external_adc.peripheral_configuration.set_cell_discharging(i, true);
		}
		else if (int(SOC::calculate(curr_cell)) - int(SOC::calculate(min_cell)) < SOC::MAX_DIFFERENCE/2){
			external_adc.peripheral_configuration.set_cell_discharging(i, false);
		}
	}
}