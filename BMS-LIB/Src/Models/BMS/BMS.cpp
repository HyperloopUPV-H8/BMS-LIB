#include "BMS/BMS.hpp"

array<BMS::COMMAND, 4> BMS::cell_voltage_registers = {READ_CELL_VOLTAGE_REGISTER_A, READ_CELL_VOLTAGE_REGISTER_B, READ_CELL_VOLTAGE_REGISTER_C, READ_CELL_VOLTAGE_REGISTER_D};

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

void BMS::read_cell_voltages() {
	uint8_t voltage_number = 0;
	for (COMMAND voltage_register : get_cell_voltage_registers()) {
		parse_voltage_group(voltage_register, voltage_number);
		voltage_number++;
	}
}

void BMS::parse_command(span<uint8_t> tx_message, COMMAND command) {
	tx_message[0] = (uint8_t)(command >> 8);
	tx_message[1] = (uint8_t)(command);
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

voltage_register_group BMS::parse_voltage_register(span<uint8_t> voltage_data) {
	return {
		(voltage_data[0] + ((uint16_t)voltage_data[1] << 8)) / 10000.0,
		(voltage_data[2] + ((uint16_t)voltage_data[3] << 8)) / 10000.0,
		(voltage_data[4] + ((uint16_t)voltage_data[5] << 8)) / 10000.0
	};
}

array<voltage_register_group, BMS::EXTERNAL_ADCS> BMS::read_voltage_register(COMMAND voltage_register) {
	const uint8_t REGISTER_LENGTH = LTC681X::DATA_REGISTER_LENGTH + PEC15::LENGTH;

	constexpr size_t voltage_data_size = REGISTER_LENGTH * EXTERNAL_ADCS;
	array<uint8_t, voltage_data_size> voltage_data;

	send_receive_command(voltage_register, voltage_data);

	static array<voltage_register_group, EXTERNAL_ADCS> voltages;
	for(int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		span adc_voltages(voltage_data.begin() + REGISTER_LENGTH * adc_number, voltage_data.begin() + REGISTER_LENGTH * (adc_number + 1));
		if (not is_pec_correct(adc_voltages)) {
			//TODO: Fault, (Protección)
		}

		voltages[adc_number] = parse_voltage_register(adc_voltages);
	}

	return voltages;
}

array<uint16_t, BMS::EXTERNAL_ADCS> BMS::get_temperatures() {
	constexpr uint8_t REGISTER_LENGTH = LTC681X::DATA_REGISTER_LENGTH + PEC15::LENGTH;
	constexpr uint8_t TEMPERATURE_DATA_SIZE = REGISTER_LENGTH*EXTERNAL_ADCS;
	array<uint8_t, TEMPERATURE_DATA_SIZE> temperature_data;

	send_receive_command(READ_STATUS_REGISTER_GROUP_A, temperature_data);
	static array<uint16_t, EXTERNAL_ADCS> temperatures;
	for(int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		span adc_temperatures(temperature_data.begin() + REGISTER_LENGTH * adc_number, temperature_data.begin() + REGISTER_LENGTH * (adc_number + 1));
		if (not is_pec_correct(adc_temperatures)) {
			//TODO: Fault, (Protección)
		}
		temperatures[adc_number] = (uint16_t)adc_temperatures[2] + ((uint16_t)adc_temperatures[3] << 8);
	}
		return temperatures;
}

void BMS::wake_up() {
	SPI::transmit(spi_instance, 1);
}

void BMS::start_adc_conversion_all_cells() {
	send_command(START_ADC_CONVERSION_ALL_CELLS);
}

void BMS::measure_internal_device_parameters() {
	send_command(MEASURE_INTERNAL_DEVICE_PARAMETERS);
}

void BMS::start_adc_conversion_gpio() {
	send_command(START_ADC_CONVERSION_ALL_GPIO);
}

void BMS::parse_voltage_group(COMMAND voltage_register, uint8_t voltage_number) {
	array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages = read_voltage_register(voltage_register);
	copy_voltages_to_external_adcs(voltages, voltage_number);
}