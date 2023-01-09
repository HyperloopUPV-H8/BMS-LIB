/*
 * PEC15.cpp
 *
 *  Created on: Dec 24, 2022
 *      Author: aleja
 */

#include "PEC15/PEC15.hpp"



uint16_t PEC15::calculate(span<uint8_t> data) {
	uint16_t remainder, addr;
	remainder = 16;

	for (uint8_t data_byte : data) {
		addr = ((remainder>>7)^data_byte)&0xff; // calculate PEC table address
		remainder = (remainder<<8)^crc15Table[addr];
	}

	return(remainder*2); // The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
};
