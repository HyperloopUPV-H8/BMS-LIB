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
