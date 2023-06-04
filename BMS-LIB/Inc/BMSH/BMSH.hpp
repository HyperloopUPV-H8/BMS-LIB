/*
 * BMSH.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "BMS/BMS.hpp"
#include "LTC6811/LTC6811.hpp"
#include "PEC15/PEC15.hpp"
#include "SOC/SOC.hpp"

class BMSH: public BMS {
private:

	void check_batteries(LTC6811& external_adc);
	span<COMMAND> get_cell_voltage_registers();
	void copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number); 
	void parse_configuration_data_stream(span<uint8_t> data_stream);
	void check_adcs();
	void deactivate_cell_discharging();
	void parse_temperatures(array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2);
	void copy_internal_temperature(array<uint16_t, BMS::EXTERNAL_ADCS> temperatures);

public:

	BMSH() = default;
	BMSH(SPI::Peripheral& spi_peripheral);

	LTC6811 external_adcs[EXTERNAL_ADCS];


	uint8_t check_adc_conversion_status();

	void update_internal_temperature();
	void start_balancing();
	void stop_balancing();
	void update_configuration();

	void initialize();

	float get_cell(uint8_t cell);
	float get_gpio(uint8_t gpio);
	LTC681X::configuration& get_config(uint8_t adc_number);

};
