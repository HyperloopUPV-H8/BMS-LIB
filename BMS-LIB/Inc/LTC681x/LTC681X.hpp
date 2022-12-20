/*
 * LTC681X.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#define DAISY_CHAIN_LENGTH 6

#include "ST-LIB.hpp"

enum ADC_MODE {
	FAST = 0b01,
	NORMAL = 0b10,
	FILTERED = 0b11
};

enum DISCHARGE {
	NOT_PERMITTED = 0,
	PERMITTED = 1
};

enum CELL_SELECTION {
	ALL = 0b000
};
class LTC681X {
public:
	LTC681X(uint8_t spi_instance);

	void wake_up();
	void send_command(uint8_t command[2]);

	void start_spi_communication();
	void start_voltage_conversion_all_cells();

private:
	uint8_t spi_instance;
	static uint16_t crc15Table[256];

	uint16_t calculate_pec15(uint8_t *data, uint8_t len);


};
