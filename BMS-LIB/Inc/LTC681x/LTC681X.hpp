/*
 * LTC681X.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"

class LTC681X {
public:
	LTC681X(uint8_t spi_instance);

	void send_command(uint8_t command[2]);

private:
	uint8_t spi_instance;
	static uint16_t crc15Table[256];

	uint16_t calculate_pec15(uint8_t len, uint8_t* data);



};
