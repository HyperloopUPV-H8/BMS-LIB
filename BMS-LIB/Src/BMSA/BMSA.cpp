/*
 * BMSA.cpp
 *
 *  Created on: 16 ene. 2023
 *      Author: aleja
 */

#include "BMSA/BMSA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

BMSA::BMSA(
		uint8_t spi_instance,
		array<reference_wrapper<Pin>, 5> dclv_pins,
		array<reference_wrapper<Pin>, 4> battery_temperature_pins,
		array<reference_wrapper<Pin>, 4> charger_temperature_pins,
		uint8_t capacitor_discharge1,
		uint8_t capacitor_discharge2
) {

	this->spi_instance = spi_instance;
	dclv_halfbridge = HalfBridge(dclv_pins[0], dclv_pins[1], dclv_pins[2], dclv_pins[3], dclv_pins[4]);
	for (Pin& battery_temperature_pin : battery_temperature_pins) {
		optional<uint8_t> optional_value = ADC::inscribe(battery_temperature_pin);
		if (not optional_value) {
			ErrorHandler("The pin %d is not in the adc_pins", battery_temperature_pin.gpio_pin);
		}
	}
}

span<BMSA::COMMAND> BMSA::get_cell_voltage_registers() {
	return span(cell_voltage_registers.begin(), cell_voltage_registers.end());
}


void BMSA::copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number) {
	for (int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		external_adc.cell_voltages[voltage_number] = voltages[adc_number];
	}
}