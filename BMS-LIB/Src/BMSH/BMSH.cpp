/*
 * BMSH.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include <BMSH/BMSH.hpp>

BMSH::COMMAND BMSH::cell_voltage_registers[6] = {READ_CELL_VOLTAGE_REGISTER_A, READ_CELL_VOLTAGE_REGISTER_B, READ_CELL_VOLTAGE_REGISTER_C, READ_CELL_VOLTAGE_REGISTER_D};


/************************************************
 *              PUBLIC FUNCTIONS
 ***********************************************/

BMSH::BMSH(uint8_t spi_instance) : spi_instance(spi_instance) {}

void BMSH::wake_up() {
	SPI::transmit(spi_instance, 1);
}

void BMSH::start_spi_communication() {
	send_command(START_SPI_COMMUNICATION);
}

void BMSH::send_command(COMMAND command) {
	constexpr uint8_t message_size = LTC6811::COMMAND_LENGTH + PEC15::LENGTH;
	array<uint8_t, message_size> tx_message;

	parse_command(tx_message, command);
	add_pec(tx_message);

	SPI::transmit(spi_instance, tx_message);
}

void BMSH::send_command(COMMAND command, span<uint8_t> data) {
	constexpr uint8_t message_size = LTC6811::COMMAND_LENGTH + PEC15::LENGTH + BMSH::DATA_STREAM;
	array<uint8_t, message_size> tx_message;

	span<uint8_t> command_with_pec(tx_message.begin(), tx_message.begin()+4);
	parse_command(command_with_pec, command);
	add_pec(command_with_pec);

	span<uint8_t> data_address{tx_message.begin()+4, tx_message.end()};
	add_message_data(data_address, data);

	SPI::transmit(spi_instance, tx_message);
}

void BMSH::start_adc_conversion_all_cells() {
	send_command(START_ADC_CONVERSION_ALL_CELLS);
}

uint8_t BMSH::check_adc_conversion_status() {
	send_command(CHECK_ADC_CONVERSION_STATUS);

	array<uint8_t, 1> adc_state;
	SPI::receive(spi_instance, adc_state);

	return(adc_state.data()[0]);
}

void BMSH::read_cell_voltages() {
	uint8_t voltage_number = 0;
	for (COMMAND voltage_register : cell_voltage_registers) {
		parse_voltage_group(voltage_register, voltage_number);
		voltage_number++;
	}
}

array<voltage_register_group, BMSH::EXTERNAL_ADCS> BMSH::read_voltage_register(COMMAND voltage_register) {
	const uint8_t REGISTER_LENGTH = LTC6811::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	send_command(voltage_register);

	constexpr size_t voltage_data_size = REGISTER_LENGTH*EXTERNAL_ADCS;
	array<uint8_t, voltage_data_size> voltage_data;
	SPI::receive(spi_instance, voltage_data);

	static array<voltage_register_group, EXTERNAL_ADCS> voltages;
	for(int adc_number : iota(EXTERNAL_ADCS)) {
		span adc_voltages(voltage_data.begin() + REGISTER_LENGTH*adc_number, voltage_data.begin() + REGISTER_LENGTH*(adc_number+1));
		if (not is_pec_correct(adc_voltages)) {
			//TODO: Error Handler
		}

		voltages[adc_number] = parse_voltage_register(adc_voltages);
	}

	return voltages;
}

void BMSH::update_cell_voltages() {
	start_adc_conversion_all_cells();

	Time::set_timeout(2, [&]() {
		read_cell_voltages();
	});
}

void BMSH::start_adc_conversion_temperatures() {
	send_command(START_ADC_CONVERSION_GPIO_1);
	send_command(START_ADC_CONVERSION_GPIO_2);
	send_command(START_ADC_CONVERSION_GPIO_3);
	send_command(START_ADC_CONVERSION_GPIO_4);
}

void BMSH::read_temperatures() {
	array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1 = read_voltage_register(READ_AUXILIARY_REGISTER_GROUP_A);
	array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2 = read_voltage_register(READ_AUXILIARY_REGISTER_GROUP_B);
	parse_temperatures(temperatures_register1, temperatures_register2);
}

void BMSH::update_temperatures() {
	start_adc_conversion_temperatures();

	Time::set_timeout(2, [&](){
		read_temperatures();
	});
}

void BMSH::start_balancing() {
	uint8_t write_buffer[256];
	for (LTC6811 external_adc : external_adcs) {
		// TODO
	}
}


/************************************************
 *              PRIVATE FUNCTIONS
 ***********************************************/

voltage_register_group BMSH::parse_voltage_register(span<uint8_t> voltage_data) {
	return {
		(uint16_t)(voltage_data[0] + ((uint16_t)voltage_data[1] << 8)),
		(uint16_t)(voltage_data[2] + ((uint16_t)voltage_data[3] << 8)),
		(uint16_t)(voltage_data[4] + ((uint16_t)voltage_data[5] << 8))
	};
}

void BMSH::parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number) {
	array<voltage_register_group, BMSH::EXTERNAL_ADCS> voltages = read_voltage_register(voltage_register);
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
		external_adcs[adc_number].cell_voltages[voltage_number] = voltages[adc_number];
	}
}

void BMSH::parse_command(span<uint8_t> tx_message, COMMAND command) {
	tx_message[0] = (uint8_t)(command >> 8);
	tx_message[1] = (uint8_t)(command);
}

void BMSH::parse_temperatures(array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2) {
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++)
	for (uint8_t adc_number : iota(EXTERNAL_ADCS)) {
		external_adcs[adc_number].temperatures[0] = temperatures_register1[adc_number];
		external_adcs[adc_number].temperatures[1] = temperatures_register2[adc_number];
	}
}

void BMSH::add_pec(span<uint8_t> data_stream) {
	span<uint8_t> data_without_pec{data_stream.begin(), data_stream.end()-2};
	uint16_t pec = PEC15::calculate(data_without_pec);
	data_stream.end()[-2] = (uint8_t)(pec >> 8);
	data_stream.end()[-1] = (uint8_t)(pec);
}

bool BMSH::is_pec_correct(span<uint8_t> data_stream){
	uint16_t calculated_pec = PEC15::calculate(span(data_stream.begin(), data_stream.end()-2));
	uint16_t received_pec = ((uint16_t)data_stream.end()[-1] << 8) | data_stream.end()[-2];

	if (calculated_pec == received_pec) {
		return true;
	} else {
		return false;
	}
}


void BMSH::add_message_data(span<uint8_t> message, span<uint8_t> data) {
	uint8_t index = 0;
	const uint8_t register_length = LTC6811::DATA_REGISTER_LENGTH;
	const uint8_t command_length = LTC6811::COMMAND_DATA_LENGTH;

	for (uint8_t external_adc : iota(0, (int)EXTERNAL_ADCS) | reverse) {
		uint8_t register_position = register_length * external_adc;
		for (uint8_t current_byte : iota(0, (int)register_length)) {
			message[index++] = data[register_position + current_byte];
		}

		add_pec(message.subspan(index-register_length, command_length));
		index += 2;
	}
}
