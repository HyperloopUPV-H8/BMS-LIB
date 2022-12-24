/*
 * PEC15.cpp
 *
 *  Created on: Dec 24, 2022
 *      Author: aleja
 */

#include "PEC15/PEC15.hpp"

namespace PEC15 {
	uint16_t calculate(uint8_t* data, uint8_t len) {
		uint16_t remainder, addr;
		remainder = 16;

		for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
		{
			addr = ((remainder>>7)^data[i])&0xff; // calculate PEC table address
			remainder = (remainder<<8)^crc15Table[addr];
		}

		return(remainder*2); // The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
	}
}
