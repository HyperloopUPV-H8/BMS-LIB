/*
 * LTC681x.cpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#include "LTC681x/LTC681X.hpp"

LTC681X::LTC681X(uint8_t spi_instance) : spi_instance(spi_instance) {
	// deactivate watchdog timer?
}

void LTC681X::wake_up() {
	uint8_t dummy_message = {0}
	RawPacket dummy_packet = RawPacket(dummy_message, 1);
}

void LTC681X::send_command(uint8_t command[2]) {
	uint8_t command_with_pec[4];
	uint16_t pec;

	command_with_pec[0] = command[0];
	command_with_pec[1] =  command[1];
	pec = calculate_pec15(2, command);
	command_with_pec[2] = (uint8_t)(pec >> 8);
	command_with_pec[3] = (uint8_t)(pec);

	RawPacket packet  = RawPacket(command_with_pec, 4);
	SPI::transmit_next_packet(spi_instance, packet);


}

uint16_t LTC681X::calculate_pec15(uint8_t len, uint8_t* data) {
	uint16_t pec;
	pec = 0b000000000010000; //initialize the PEC
	// TODO: pec function
	return 0;



}
