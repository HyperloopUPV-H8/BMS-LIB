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


	span<COMMAND> get_cell_voltage_registers();
	void copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number); 
	void parse_configuration_data_stream(span<uint8_t> data_stream);
	void check_adcs();
	void deactivate_cell_discharging();\
	void parse_temperatures(array<voltage_register_group, BMSA::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSA::EXTERNAL_ADCS> temperatures_register2);
	void copy_internal_temperature(array<uint16_t, BMS::EXTERNAL_ADCS> temperatures);

public:
	LTC6810 external_adc;

	void initialize();
	
	BMSA() = default;
	BMSA(SPI::Peripheral& spi_peripheral);
};
