/*
 * BMSH.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "LTC6811/LTC6811.hpp"
#include "PEC15/PEC15.hpp"

using std::views::iota;
using std::views::reverse;

class BMSH {
private:
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
	static const uint8_t EXTERNAL_ADCS = 5;
	static const uint8_t DATA_STREAM = LTC6811::COMMAND_DATA_LENGTH*EXTERNAL_ADCS;

public:
	enum COMMAND : uint16_t {
		START_SPI_COMMUNICATION = 0b11100100011,

		READ_CELL_VOLTAGE_REGISTER_A = 0b0100,
		READ_CELL_VOLTAGE_REGISTER_B = 0b0110,
		READ_CELL_VOLTAGE_REGISTER_C = 0b1000,
		READ_CELL_VOLTAGE_REGISTER_D = 0b1010,
		READ_CELL_VOLTAGE_REGISTER_E = 0b1001,
		READ_CELL_VOLTAGE_REGISTER_F = 0b1011,

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

private:
	uint8_t spi_instance;
	static COMMAND cell_voltage_registers[6];
	array<voltage_register_group, BMSH::EXTERNAL_ADCS> read_voltage_register(COMMAND voltage_register);
	LTC6811 external_adcs[EXTERNAL_ADCS];

	voltage_register_group parse_voltage_register(span<uint8_t> voltage_data);
	void parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number);
	void parse_command(span<uint8_t> tx_message, COMMAND command);
	void parse_temperatures(array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2);

	void add_pec(span<uint8_t> data_stream, uint8_t len);
	bool is_pec_correct(span<uint8_t> data_stream);

	void add_message_data(span<uint8_t> message, span<uint8_t> data);

	void start_adc_conversion_all_cells();
	uint8_t check_adc_conversion_status();
	void read_cell_voltages();

	void start_adc_conversion_temperatures();
	void read_temperatures();

public:

	BMSH(uint8_t spi_instance);

	void wake_up();
	void start_spi_communication();
	void send_command(COMMAND command);
	void send_command(COMMAND command, span<uint8_t> tx_data);

	void update_cell_voltages();
	void update_temperatures();

	void start_balancing();
};
