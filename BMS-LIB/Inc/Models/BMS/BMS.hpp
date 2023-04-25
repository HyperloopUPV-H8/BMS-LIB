/*
 * BMS.hpp
 *
 *  Created on: 14 ene. 2023
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "LTC681X/LTC681X.hpp"
#include "Battery/Battery.hpp"

using std::begin;
using std::end;
using std::copy;

using std::views::iota;
using std::views::reverse;
using std::views::take;
using std::views::drop;

class BMS {
public:
	static const int EXTERNAL_ADCS = 5;
	bool balancing = false;
protected:
	enum ADC_MODE : uint8_t {
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
	static const uint8_t DATA_STREAM = LTC681X::COMMAND_DATA_LENGTH*EXTERNAL_ADCS;

	void add_pec(span<uint8_t> data_stream);
	bool is_pec_correct(span<uint8_t> data_stream);


public:
	enum COMMAND : uint16_t {
		START_SPI_COMMUNICATION = 0b11100100011,

		READ_CELL_VOLTAGE_REGISTER_A = 0b0100,
		READ_CELL_VOLTAGE_REGISTER_B = 0b0110,
		READ_CELL_VOLTAGE_REGISTER_C = 0b1000,
		READ_CELL_VOLTAGE_REGISTER_D = 0b1010,

		WRITE_CONFIGURATION_REGISTER_GROUP = 0b1,
		READ_CONFIGURATION_REGISTER_GROUP = 0b10,


		READ_AUXILIARY_REGISTER_GROUP_A = 0b00000001100,
		READ_AUXILIARY_REGISTER_GROUP_B = 0b00000001110,

		READ_STATUS_REGISTER_GROUP_A = 0b00000010000,
		MEASURE_INTERNAL_DEVICE_PARAMETERS = 0b10001101000 | (ADC_MODE::NORMAL << 7),

		CHECK_ADC_CONVERSION_STATUS = 0b11100010100,

		START_ADC_CONVERSION_ALL_CELLS = 0b1001100000 | (ADC_MODE::NORMAL << 7) | (DISCHARGE::PERMITTED << 4),
		START_ADC_CONVERSION_ALL_GPIO = 0b10001100000 | (ADC_MODE::NORMAL << 7),
		START_ADC_CONVERSION_GPIO_1 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_1),
		START_ADC_CONVERSION_GPIO_2 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_2),
		START_ADC_CONVERSION_GPIO_3 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_3),
		START_ADC_CONVERSION_GPIO_4 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_4),
		START_ADC_CONVERSION_GPIO_5 = 0b10001100000 | (ADC_MODE::NORMAL << 7) | (GPIO_SELECTION::GPIO_5),


	};

	void send_command(COMMAND command);
	void send_command(COMMAND command, span<uint8_t> tx_data);
	void send_receive_command(COMMAND command, span<uint8_t> rx_data);
	void send_receive_command(COMMAND command, span<uint8_t> tx_data, span<uint8_t> rx_data);

protected:
	uint8_t spi_instance;
	array<voltage_register_group, BMS::EXTERNAL_ADCS> read_voltage_register(COMMAND voltage_register);
	array<uint16_t, BMS::EXTERNAL_ADCS> get_temperatures();

	voltage_register_group parse_voltage_register(span<uint8_t> voltage_data);

	void add_message_data(span<uint8_t> message, span<uint8_t> data);

	void parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number);
	void parse_command(span<uint8_t> tx_message, COMMAND command);
	void parse_temperatures(array<voltage_register_group, BMS::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMS::EXTERNAL_ADCS> temperatures_register2);
};
