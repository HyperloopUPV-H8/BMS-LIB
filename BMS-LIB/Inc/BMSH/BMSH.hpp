/*
 * BMSH.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "BMS/BMS.hpp"
#include "ST-LIB.hpp"
#include "LTC6811/LTC6811.hpp"
#include "PEC15/PEC15.hpp"

using std::begin;
using std::end;
using std::copy;

using std::views::iota;
using std::views::reverse;
using std::views::take;
using std::views::drop;

class BMSH: public BMS {
private:
	uint8_t spi_instance;

	static COMMAND cell_voltage_registers[6];
	array<voltage_register_group, BMSH::EXTERNAL_ADCS> read_voltage_register(COMMAND voltage_register);

	LTC6811 external_adcs[EXTERNAL_ADCS];

	voltage_register_group parse_voltage_register(span<uint8_t> voltage_data);
	void parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number);
	void parse_command(span<uint8_t> tx_message, COMMAND command);
	void parse_temperatures(array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2);

	void add_pec(span<uint8_t> data_stream);
	bool is_pec_correct(span<uint8_t> data_stream);

	void add_message_data(span<uint8_t> message, span<uint8_t> data);

	void start_adc_conversion_all_cells();
	uint8_t check_adc_conversion_status();
	void read_cell_voltages();

	void start_adc_conversion_temperatures();
	void read_temperatures();

	void check_batteries(LTC6811 external_adc);

public:

	BMSH(uint8_t spi_instance);

	void wake_up();
	void start_spi_communication();
	void send_command(COMMAND command);
	void send_command(COMMAND command, span<uint8_t> tx_data);

	void update_cell_voltages();
	void update_temperatures();
	void start_balancing();
	void update_configuration();
};
