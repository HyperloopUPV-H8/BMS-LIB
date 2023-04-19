/*
 * BMSA.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "BMS/BMS.hpp"

class BMSA : public BMS {
private:
	HalfBridge dclv_halfbridge;
	array<uint8_t, 4> battery_temperatures;
	array<uint8_t, 4> charger_temperatures;
	uint8_t capacitor_discharge1;
	uint8_t capacitor_discharge2;

public:
	BMSA(
			uint8_t spi_instance,
			array<reference_wrapper<Pin>, 5> dclv_pins,
			array<reference_wrapper<Pin>, 4> battery_temperature_pins,
			array<reference_wrapper<Pin>, 4> charger_temperature_pins,
			uint8_t capacitor_discharge1,
			uint8_t capacitor_discharge2
	);
};
