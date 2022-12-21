/*
 * LTC681X.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#pragma once

#define DAISY_CHAIN_LENGTH 5
#define REGISTER_GROUPS 4

#define MINIMUM_CELL_VOLTAGE 3.6 // Ask Hardware
#define MAXIMUM_CELL_VOLTAGE 4.2

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

enum VOLTAGE_REGISTER_COMMAND : uint8_t {
	A = 0b0100,
	B = 0b0110,
	C = 0b1000,
	D = 0b1010,
	E = 0b1001,
	F = 0b1011
};

struct voltage_register_group {
	uint16_t cell1_voltage;
	uint16_t cell2_voltage;
	uint16_t cell3_voltage;
};
enum COMMAND : uint16_t {
	READ_CELL_VOLTAGE_REGISTER_A = A,
	READ_CELL_VOLTAGE_REGISTER_B = B,
	READ_CELL_VOLTAGE_REGISTER_C = C,
	READ_CELL_VOLTAGE_REGISTER_D = D,
	READ_CELL_VOLTAGE_REGISTER_E = E,
	READ_CELL_VOLTAGE_REGISTER_F = F,

	CHECK_ADC_CONVERSION_STATUS = 0b11100010100,

	START_VOLTAGE_CONVERSION_ALL_CELLS = 0b01001100000
										 | ((ADC_MODE::NORMAL & 0b10) << 8) // revisar
										 | ((ADC_MODE::NORMAL & 0b1) << 7)
										 | (DISCHARGE::PERMITTED << 4),

	START_SPI_COMMUNICATION = 0b11100100011,
};

class LTC681X {
public:
	LTC681X(uint8_t spi_instance);

	void wake_up();
	void send_command(COMMAND command);

	void start_spi_communication();
	void start_voltage_conversion_all_cells();
	uint8_t check_adc_conversion_status();
	void read_voltages(voltage_register_group cell_voltages_array[REGISTER_GROUPS]);
	voltage_register_group read_cell_voltage_register(COMMAND voltage_register);
	void get_cell_voltages(voltage_register_group cell_voltages_array[REGISTER_GROUPS]);


private:
	uint8_t spi_instance;
	static uint16_t crc15Table[256];
	static COMMAND voltage_registers[6];

	uint16_t calculate_pec15(uint8_t *data, uint8_t len);


};
