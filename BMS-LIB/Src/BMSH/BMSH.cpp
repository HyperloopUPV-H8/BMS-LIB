/*
 * BMSH.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include <BMSH/BMSH.hpp>

BMSH::COMMAND BMSH::cell_voltage_registers[6] = {READ_CELL_VOLTAGE_REGISTER_A, READ_CELL_VOLTAGE_REGISTER_B, READ_CELL_VOLTAGE_REGISTER_C, READ_CELL_VOLTAGE_REGISTER_D, READ_CELL_VOLTAGE_REGISTER_E, READ_CELL_VOLTAGE_REGISTER_F};

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

	tx_message[0] = (uint8_t)(command >> 8);
	tx_message[1] = (uint8_t)(command);
	add_pec(tx_message, LTC6811::COMMAND_LENGTH);

	RawPacket packet = RawPacket(tx_message, message_size);
	SPI::transmit_next_packet(spi_instance, packet);
}

void BMSH::send_command(COMMAND command, uint8_t* data) {
	uint8_t message_size = LTC6811::COMMAND_LENGTH + PEC15::LENGTH + DATA_STREAM;
	uint8_t tx_message[message_size];

	tx_message[0] = (uint8_t)(command >> 8);
	tx_message[1] = (uint8_t)(command);
	add_pec(tx_message, LTC6811::COMMAND_LENGTH);

	uint8_t command_index = 4;
	for (uint8_t external_adc=EXTERNAL_ADCS-1; external_adc >= 0; external_adc--) {
		uint8_t register_position = LTC6811::DATA_REGISTER_LENGTH*external_adc;
		for (uint8_t current_byte=0; current_byte < LTC6811::DATA_REGISTER_LENGTH; current_byte++) {
			tx_message[command_index++] = data[register_position + current_byte];
		}
		add_pec(&data[register_position], LTC6811::COMMAND_DATA_LENGTH);
		command_index += 2;
	}

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
		voltage_register_group* voltages = read_voltage_register(voltage_register);
		for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
			external_adcs[adc_number].cell_voltages[register_number] = voltages[adc_number];
		}
	}
}

voltage_register_group* BMSH::read_voltage_register(COMMAND voltage_register) {
	const uint8_t REGISTER_LENGTH = LTC6811::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	send_command(voltage_register);

	RawPacket cell_codes = RawPacket(REGISTER_LENGTH*EXTERNAL_ADCS);
	SPI::receive_next_packet(spi_instance, cell_codes);

	uint8_t* voltage_data = cell_codes.get_data();
	if (not is_pec_correct(voltage_data, REGISTER_LENGTH)) {
		//TODO: Error Handler
	}

	static voltage_register_group voltages[EXTERNAL_ADCS];
	for(int adc_number=0; adc_number<EXTERNAL_ADCS;adc_number++){
		voltages[adc_number] = {
			voltage_data[0] + ((uint16_t)voltage_data[1] << 8),
			voltage_data[2] + ((uint16_t)voltage_data[3] << 8),
			voltage_data[4] + ((uint16_t)voltage_data[5] << 8)
		};
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
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
		external_adcs[adc_number].temperatures[0] = temperatures_register1[adc_number];
		external_adcs[adc_number].temperatures[1] = temperatures_register2[adc_number];
	}
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
