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

	LTC6810 external_adc;

	span<COMMAND> get_cell_voltage_registers();
	void copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number); 

public:
	BMSA() = default;
	BMSA(SPI::Peripheral& spi_peripheral);
};
