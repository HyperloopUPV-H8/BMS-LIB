/*
 * BMSH.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#define EXTERNAL_ADCS 5

#include "ST-LIB.hpp"
#include "LTC6811/LTC6811.hpp"
#include "PEC15/PEC15.hpp"

enum ADC_MODE {
	FAST = 0b01,
	NORMAL = 0b10,
	FILTERED = 0b11
};
enum GPIO_SELECTION {
	ALL = 0b000,
	GPIO_1 = 0b001,
	GPIO_2 = 0b010,
	GPIO_3 = 0b011,
	GPIO_4 = 0b100,
	GPIO_5 = 0b101,
	SECOND_REFERENCE = 0b110
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
enum COMMAND : uint16_t {
	START_SPI_COMMUNICATION = 0b11100100011,

	READ_CELL_VOLTAGE_REGISTER_A = A,
	READ_CELL_VOLTAGE_REGISTER_B = B,
	READ_CELL_VOLTAGE_REGISTER_C = C,
	READ_CELL_VOLTAGE_REGISTER_D = D,
	READ_CELL_VOLTAGE_REGISTER_E = E,
	READ_CELL_VOLTAGE_REGISTER_F = F,

	READ_AUXILIARY_REGISTER_GROUP_A = 0b00000001100,
	READ_AUXILIARY_REGISTER_GROUP_B = 0b00000001110,

	CHECK_ADC_CONVERSION_STATUS = 0b11100010100,

	START_ADC_CONVERSION_ALL_CELLS = 0b01001100000 | (ADC_MODE::NORMAL << 7) | (DISCHARGE::PERMITTED << 4),
	START_ADC_CONVERSION_GPIO_1 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_1),
	START_ADC_CONVERSION_GPIO_2 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_2),
	START_ADC_CONVERSION_GPIO_3 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_3),
	START_ADC_CONVERSION_GPIO_4 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_4),
	START_ADC_CONVERSION_GPIO_5 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_5),
};

class BMSH {
public:
	BMSH(uint8_t spi_instance);

	void wake_up();
	void start_spi_communication();
	void send_command(COMMAND command);

	void start_adc_conversion_all_cells();
	uint8_t check_adc_conversion_status();
	void read_cell_voltages();
	void update_cell_voltages();

	void start_adc_conversion_temperatures();
	void read_temperatures();
	void update_temperatures();

	LTC6811 external_adcs[EXTERNAL_ADCS];

private:
	uint8_t spi_instance;
	static COMMAND cell_voltage_registers[6];
	voltage_register_group* read_voltage_register(COMMAND voltage_register);
};
