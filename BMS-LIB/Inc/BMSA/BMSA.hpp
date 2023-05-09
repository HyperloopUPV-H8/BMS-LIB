/*
 * BMSA.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "BMS/BMS.hpp"
#include "LTC6810/LTC6810.hpp"

class BMSA : public BMS {
private:
	HalfBridge dclv_halfbridge;
	array<uint8_t, 4> charger_temperatures;
	uint8_t capacitor_discharge1;
	uint8_t capacitor_discharge2;

	LTC6810 external_adc;

	span<COMMAND> get_cell_voltage_registers();
	void copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number); 

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
