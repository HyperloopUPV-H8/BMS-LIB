/*
 * BMSH.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include <BMSH/BMSH.hpp>

BMSH::COMMAND BMSH::cell_voltage_registers[4] = {READ_CELL_VOLTAGE_REGISTER_A, READ_CELL_VOLTAGE_REGISTER_B, READ_CELL_VOLTAGE_REGISTER_C, READ_CELL_VOLTAGE_REGISTER_D};


/************************************************
 *              PUBLIC FUNCTIONS
 ***********************************************/

BMSH::BMSH(SPI::Peripheral& spi_peripheral) {
	optional<uint8_t> spi_optional = SPI::inscribe(spi_peripheral);

	if (not spi_optional) {
		ErrorHandler("SPI peripheral of BMSH did not register correctly");
		return;
	}

	spi_instance = spi_optional.value();
}

void BMSH::wake_up() {
	SPI::transmit(spi_instance, 1);
}


void BMSH::start_adc_conversion_all_cells() {
	send_command(START_ADC_CONVERSION_ALL_CELLS);
}

void BMSH::measure_internal_device_parameters() {
	send_command(MEASURE_INTERNAL_DEVICE_PARAMETERS);
}

void BMSH::start_adc_conversion_gpio() {
	send_command(START_ADC_CONVERSION_ALL_GPIO);
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

void BMSH::read_internal_temperature() {
	array<uint16_t, BMSH::EXTERNAL_ADCS> temperatures = get_temperatures();
	for (int adc_number=0; adc_number<EXTERNAL_ADCS; adc_number++) {
		external_adcs[adc_number].internal_temperature = temperatures[adc_number] * (0.0001 / 0.0075) - 273;
	}
}

//TODO: revisar
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

//TODO:
void BMSH::start_balancing() {
	for (uint8_t i : iota(0, (int)BMS::EXTERNAL_ADCS)) {
		check_batteries(external_adcs[i]);
	}

	wake_up();
	update_configuration();
}

void BMSH::update_configuration(){
	constexpr uint8_t data_size = LTC6811::DATA_REGISTER_LENGTH * EXTERNAL_ADCS;
	array<uint8_t, data_size> data_stream = { 0 };
	uint8_t offset = 0;
	for (LTC6811 external_adc : external_adcs) {
		for (bitset<8> data_register : external_adc.peripheral_configuration.register_group) {
			data_stream[offset] = (uint8_t)data_register.to_ulong(); //TODO: Refactor this, is horrible.
			offset++;
		}
	}
	send_command(WRITE_CONFIGURATION_REGISTER_GROUP, data_stream);
}

LTC681X::configuration& BMSH::get_config(uint8_t adc_number) {
	return external_adcs[adc_number].peripheral_configuration;
}
/************************************************
 *              PRIVATE FUNCTIONS
 ***********************************************/

void BMSH::check_batteries(LTC6811& external_adc) {
	uint8_t cell_offset = 0;
	for (Battery battery : external_adc.batteries) {

		if(not battery.needs_balance()) {
			cell_offset += 6;
			continue;
		}

		for(uint8_t i : iota(0, (int)Battery::CELLS)) {
			uint16_t& min_cell = *battery.minimum_cell;
			uint16_t& curr_cell = *battery.cells[i];
			if (int(SOC::calculate(curr_cell)) - int(SOC::calculate(min_cell)) > SOC::MAX_DIFFERENCE) {
				external_adc.peripheral_configuration.set_cell_discharging(cell_offset+i, true);
			}
			else if (int(SOC::calculate(curr_cell)) - int(SOC::calculate(min_cell)) < SOC::MAX_DIFFERENCE/2){
				external_adc.peripheral_configuration.set_cell_discharging(cell_offset+i, false);
			}
		}

		cell_offset += 6;
	}
}

float BMSH::get_cell(uint8_t cell) {
	LTC6811& adc = external_adcs[cell/12];
	Battery& battery = adc.batteries[cell%12/6];
	return *battery.cells[cell%12%6] / 10000.0;
}

float BMSH::get_gpio(uint8_t gpio) {
	LTC6811& adc = external_adcs[gpio/4];
	Battery& battery = adc.batteries[gpio%4/2];
	if (gpio%4%2 == 0) {
		return *battery.temperature1;
	} else {
		return *battery.temperature2;
	}
}
