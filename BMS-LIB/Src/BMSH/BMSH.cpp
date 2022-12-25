/*
 * BMSH.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include <BMSH/BMSH.hpp>

BMSH::COMMAND BMSH::cell_voltage_registers[6] = {READ_CELL_VOLTAGE_REGISTER_A, READ_CELL_VOLTAGE_REGISTER_B, READ_CELL_VOLTAGE_REGISTER_C, READ_CELL_VOLTAGE_REGISTER_D, READ_CELL_VOLTAGE_REGISTER_E, READ_CELL_VOLTAGE_REGISTER_F};


/************************************************
 *              PUBLIC FUNCTIONS
 ***********************************************/

BMSH::BMSH(uint8_t spi_instance) : spi_instance(spi_instance) {

}

void BMSH::wake_up() {
	RawPacket dummy_packet = RawPacket(1);
	SPI::transmit_next_packet(spi_instance, dummy_packet);
}

void BMSH::start_spi_communication() {
	send_command(START_SPI_COMMUNICATION);
}

void BMSH::send_command(COMMAND command) {
	uint8_t message_size = LTC6811::COMMAND_LENGTH + PEC15::LENGTH;
	uint8_t tx_message[message_size];

	parse_command(tx_message, command);
	add_pec(tx_message, LTC6811::COMMAND_LENGTH);

	RawPacket packet = RawPacket(tx_message, message_size);
	SPI::transmit_next_packet(spi_instance, packet);
}

void BMSH::send_command(COMMAND command, uint8_t* data) {
	uint8_t message_size = LTC6811::COMMAND_LENGTH + PEC15::LENGTH + DATA_STREAM;
	uint8_t tx_message[message_size];

	parse_command(tx_message, command);
	add_pec(tx_message, LTC6811::COMMAND_LENGTH);

	add_message_data(&tx_message[4], data);

	RawPacket packet = RawPacket(tx_message, LTC6811::COMMAND_LENGTH + PEC15::LENGTH + DATA_STREAM);
	SPI::transmit_next_packet(spi_instance, packet);
}

void BMSH::start_adc_conversion_all_cells() {
	send_command(START_ADC_CONVERSION_ALL_CELLS);
}

uint8_t BMSH::check_adc_conversion_status() {
	send_command(CHECK_ADC_CONVERSION_STATUS);

	RawPacket adc_state = RawPacket(1);
	SPI::receive_next_packet(spi_instance, adc_state);

	return(adc_state.get_data()[0]);
}

void BMSH::read_cell_voltages() {
	uint8_t register_number = 0;
	for (COMMAND voltage_register : cell_voltage_registers) {
		parse_voltage_group(voltage_register, register_number);
		register_number++;
	}
}

voltage_register_group* BMSH::read_voltage_register(COMMAND voltage_register) {
	const uint8_t REGISTER_LENGTH = LTC6811::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	send_command(voltage_register);

	RawPacket cell_codes = RawPacket(REGISTER_LENGTH*EXTERNAL_ADCS);
	SPI::receive_next_packet(spi_instance, cell_codes);

	uint8_t* voltage_data = cell_codes.get_data();

	static voltage_register_group voltages[EXTERNAL_ADCS];
	for(int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++){
		uint8_t* adc_address = &voltage_data[REGISTER_LENGTH*adc_number];
		if (not is_pec_correct(adc_address, REGISTER_LENGTH)) {
			//TODO: Error Handler
		}

		voltages[adc_number] = parse_voltage_register(adc_address);
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
	voltage_register_group* temperatures_register1 = read_voltage_register(READ_AUXILIARY_REGISTER_GROUP_A);
	voltage_register_group* temperatures_register2 = read_voltage_register(READ_AUXILIARY_REGISTER_GROUP_B);
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

voltage_register_group BMSH::parse_voltage_register(uint8_t* voltage_data) {
	return {
		voltage_data[0] + ((uint16_t)voltage_data[1] << 8),
		voltage_data[2] + ((uint16_t)voltage_data[3] << 8),
		voltage_data[4] + ((uint16_t)voltage_data[5] << 8)
	};
}

void BMSH::parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number) {
	voltage_register_group* voltages = read_voltage_register(voltage_register);
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
		external_adcs[adc_number].cell_voltages[voltage_number] = voltages[adc_number];
	}
}

void BMSH::parse_command(uint8_t* tx_message, COMMAND command) {
	tx_message[0] = (uint8_t)(command >> 8);
	tx_message[1] = (uint8_t)(command);
}

void BMSH::parse_temperatures(voltage_register_group* temperatures_register1, voltage_register_group* temperatures_register2) {
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
		external_adcs[adc_number].temperatures[0] = temperatures_register1[adc_number];
		external_adcs[adc_number].temperatures[1] = temperatures_register2[adc_number];
	}
}

void BMSH::add_pec(uint8_t* data_stream, uint8_t len) {
	uint16_t pec = PEC15::calculate(data_stream, LTC6811::COMMAND_LENGTH);
	data_stream[len] = (uint8_t)(pec >> 8);
	data_stream[len+1] = (uint8_t)(pec);
}

bool BMSH::is_pec_correct(uint8_t* data_stream, uint8_t len) {
	uint16_t calculated_pec = PEC15::calculate(data_stream, len-PEC15::LENGTH);
	uint16_t received_pec = ((uint16_t)data_stream[len] << 8) | data_stream[len+1];

	if (calculated_pec == received_pec) {
		return true;
	} else {
		return false;
	}
}

void BMSH::add_message_data(uint8_t* message, uint8_t* data) {
	uint8_t index;
	for (uint8_t external_adc=EXTERNAL_ADCS-1; external_adc >= 0; external_adc--) {
		uint8_t register_position = LTC6811::DATA_REGISTER_LENGTH*external_adc;
		for (uint8_t current_byte=0; current_byte < LTC6811::DATA_REGISTER_LENGTH; current_byte++) {
			message[index++] = data[register_position + current_byte];
		}
		add_pec(&data[register_position], LTC6811::COMMAND_DATA_LENGTH);
		index += 2;
	}
}
