#include "BMS/BMS.hpp"

void BMS::send_command(COMMAND command) {
	constexpr uint8_t message_size = LTC681X::COMMAND_LENGTH + PEC15::LENGTH;
	array<uint8_t, message_size> tx_message;

	parse_command(tx_message, command);
	add_pec(tx_message);

	SPI::transmit(spi_instance, tx_message);
}

void BMS::send_command(COMMAND command, span<uint8_t> data) {
	constexpr uint8_t message_size = LTC681X::COMMAND_LENGTH + PEC15::LENGTH + BMS::DATA_STREAM;
	array<uint8_t, message_size> tx_message;

	span<uint8_t> command_with_pec(tx_message.begin(), tx_message.begin()+4);
	parse_command(command_with_pec, command);
	add_pec(command_with_pec);

	span<uint8_t> data_address{tx_message.begin()+4, tx_message.end()};
	add_message_data(data_address, data);

	SPI::transmit(spi_instance, tx_message);
}

void BMS::send_receive_command(COMMAND command, span<uint8_t> rx) {
	constexpr uint8_t message_size =LTC681X::COMMAND_LENGTH  + PEC15::LENGTH;
	array<uint8_t, message_size> tx_message = { 0 };

	parse_command(tx_message, command);
	add_pec(tx_message);

	SPI::transmit_and_receive(spi_instance, tx_message, rx);
}

void BMS::send_receive_command(COMMAND command, span<uint8_t> tx_data, span<uint8_t> rx) {
	constexpr uint8_t message_size = LTC681X::COMMAND_LENGTH + PEC15::LENGTH + BMS::DATA_STREAM;
	array<uint8_t, message_size> tx_message;

	span<uint8_t> command_with_pec(tx_message.begin(), tx_message.begin()+4);
	parse_command(command_with_pec, command);
	add_pec(command_with_pec);

	span<uint8_t> data_address{tx_message.begin()+4, tx_message.end()};
	add_message_data(data_address, tx_data);

	SPI::transmit_and_receive(spi_instance, tx_message, rx);
}

void BMS::add_pec(span<uint8_t> data_stream) {
	span<uint8_t> data_without_pec{data_stream.begin(), data_stream.end()-2};
	uint16_t pec = PEC15::calculate(data_without_pec);
	data_stream.end()[-2] = (uint8_t)(pec >> 8);
	data_stream.end()[-1] = (uint8_t)(pec);
}

bool BMS::is_pec_correct(span<uint8_t> data_stream){
	uint16_t calculated_pec = PEC15::calculate(span(data_stream.begin(), data_stream.end()-2));
	uint16_t received_pec = ((uint16_t)data_stream.end()[-1] << 8) | data_stream.end()[-2];

	if (calculated_pec == received_pec) {
		return true;
	} else {
		return false;
	}
}

void BMS::add_message_data(span<uint8_t> message, span<uint8_t> data) {
	uint8_t index = 0;
	const uint8_t register_length = LTC681X::DATA_REGISTER_LENGTH;
	const uint8_t command_length = LTC681X::COMMAND_DATA_LENGTH;

	for (uint8_t external_adc : iota(0, EXTERNAL_ADCS) | reverse) {
		uint8_t register_position = register_length * external_adc;
		for (uint8_t current_byte : iota(0, (int)register_length)) {
			message[index++] = data[register_position + current_byte];
		}

		add_pec(message.subspan(index-register_length, command_length));
		index += 2;
	}
}

array<uint16_t, BMS::EXTERNAL_ADCS> BMS::get_temperatures() {
	const uint8_t REGISTER_LENGTH = LTC681X::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	constexpr size_t status_data_size = REGISTER_LENGTH*EXTERNAL_ADCS;
	array<uint8_t, status_data_size> status_data = { 0 };

	send_receive_command(READ_STATUS_REGISTER_GROUP_A, status_data);

	static array<uint16_t, EXTERNAL_ADCS> temperatures;
	for(int adc_number : iota(0, (int)EXTERNAL_ADCS)) {
		span adc_status(status_data.begin() + REGISTER_LENGTH * adc_number, status_data.begin() + REGISTER_LENGTH * (adc_number + 1));
		if (not is_pec_correct(adc_status)) {
			//TODO: Fault, (Protección)
		}

		temperatures[adc_number] = adc_status[2] + (((uint16_t)adc_status[3])<<8);
	}

	return temperatures;
}

array<voltage_register_group, BMS::EXTERNAL_ADCS> BMS::read_voltage_register(COMMAND voltage_register) {
	const uint8_t REGISTER_LENGTH = LTC681X::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	constexpr size_t voltage_data_size = REGISTER_LENGTH*EXTERNAL_ADCS;
	array<uint8_t, voltage_data_size> voltage_data = { 0 };

	send_receive_command(voltage_register, voltage_data);

	static array<voltage_register_group, EXTERNAL_ADCS> voltages;
	for(int adc_number : iota(0, (int)EXTERNAL_ADCS)) {
		span adc_voltages(voltage_data.begin() + REGISTER_LENGTH * adc_number, voltage_data.begin() + REGISTER_LENGTH * (adc_number + 1));
		if (not is_pec_correct(adc_voltages)) {
			//TODO: Fault, (Protección)
		}

		voltages[adc_number] = parse_voltage_register(adc_voltages);
	}

	return voltages;
}
