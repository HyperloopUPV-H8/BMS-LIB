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

	static COMMAND cell_voltage_registers[4];

	void check_batteries(LTC6811& external_adc);

public:

	BMSH(SPI::Peripheral& spi_peripheral);

	LTC6811 external_adcs[EXTERNAL_ADCS];

	void wake_up();

	void start_adc_conversion_all_cells();
	void measure_internal_device_parameters();
	void start_adc_conversion_gpio();
	uint8_t check_adc_conversion_status();
	void read_cell_voltages();
	void read_internal_temperature();

	void start_adc_conversion_temperatures();
	void read_temperatures();

	void update_cell_voltages();
	void update_temperatures();
	void start_balancing();
	void stop_balancing();
	void update_configuration();

	float get_cell(uint8_t cell);
	float get_gpio(uint8_t gpio);
	LTC681X::configuration& get_config(uint8_t adc_number);

};
