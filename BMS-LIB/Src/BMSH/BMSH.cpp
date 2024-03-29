/*
 * BMSH.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include <BMSH/BMSH.hpp>



/************************************************
 *              PUBLIC FUNCTIONS
 ***********************************************/

BMSH::BMSH(SPI::Peripheral& spi_peripheral) {
	spi_instance = SPI::inscribe(spi_peripheral);


	external_adcs[0] = LTC6811();
	external_adcs[1] = LTC6811();
	external_adcs[2] = LTC6811();
	external_adcs[3] = LTC6811();
	external_adcs[4] = LTC6811();
}

void BMSH::initialize() {
	external_adcs[0].initialize_batteries();
	external_adcs[1].initialize_batteries();
	external_adcs[2].initialize_batteries();
	external_adcs[3].initialize_batteries();
	external_adcs[4].initialize_batteries();
}

uint8_t BMSH::check_adc_conversion_status() {
	send_command(CHECK_ADC_CONVERSION_STATUS);

	array<uint8_t, 1> adc_state;
	SPI::receive(spi_instance, adc_state);

	return(adc_state.data()[0]);
}

span<BMS::COMMAND> BMSH::get_cell_voltage_registers() {
	return cell_voltage_registers;
}

void BMSH::deactivate_cell_discharging() {
	for (uint8_t i : iota(0, BMSH::EXTERNAL_ADCS)) {
		for (uint8_t j : iota(0, 2*Battery::CELLS)) {
			external_adcs[i].peripheral_configuration.set_cell_discharging(j, false);
		}
	}
}


LTC681X::configuration& BMSH::get_config(uint8_t adc_number) {
	return external_adcs[adc_number].peripheral_configuration;
}

/************************************************
 *              PRIVATE FUNCTIONS
 ***********************************************/

void BMSH::check_adcs() {
	for (uint8_t i : iota(0, BMS::EXTERNAL_ADCS)) {
		check_batteries(external_adcs[i]);
	}
}

void BMSH::copy_internal_temperature(array<uint16_t, BMS::EXTERNAL_ADCS> temperatures) {
	for (uint8_t i : iota(0, BMS::EXTERNAL_ADCS)) {
		external_adcs[i].internal_temperature = (float)temperatures[i] / 75 - 273;
	}
}

void BMSH::parse_configuration_data_stream(span<uint8_t> data_stream) {
	uint8_t offset = 0;
	for (LTC6811 external_adc : external_adcs) {
		for (bitset<8> data_register : external_adc.peripheral_configuration.register_group) {
			data_stream[offset] = (uint8_t)data_register.to_ulong(); //TODO: Refactor this, is horrible.
			offset++;
		}
	}
}

void BMSH::check_batteries(LTC6811& external_adc) {
	uint8_t cell_offset = 0;
	for (Battery battery : external_adc.batteries) {

		if(not battery.needs_balance()) {
			cell_offset += 6;
			continue;
		}

		for(uint8_t i : iota(0, (int)Battery::CELLS)) {
			float& min_cell = battery.minimum_cell;
			float& curr_cell = *battery.cells[i];
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
	return *battery.cells[cell%12%6];
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

float BMSH::get_total_voltage() {
	float total_voltage = 0;
	for (LTC6811& adc: external_adcs) {
		for (Battery& battery: adc.batteries) {
			if (battery.is_connected) {
				total_voltage += battery.total_voltage;
			}
		}
	}
	
	return total_voltage;
}


void BMSH::parse_temperatures(array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSH::EXTERNAL_ADCS> temperatures_register2) {
	for (int adc_number : iota(0, EXTERNAL_ADCS)) {
			int raw_temp = (temperatures_register1[adc_number].voltage1 * 10000) / 16;
			external_adcs[adc_number].temperatures[0].voltage1 = NTC_table[raw_temp]*0.1 / 2;

			raw_temp = (temperatures_register1[adc_number].voltage2 * 10000) / 16;
			external_adcs[adc_number].temperatures[0].voltage2 = NTC_table[raw_temp]*0.1 / 2;

			raw_temp = (temperatures_register1[adc_number].voltage3 * 10000) / 16;
			external_adcs[adc_number].temperatures[0].voltage3 = NTC_table[raw_temp]*0.1 / 2;

			raw_temp = (temperatures_register2[adc_number].voltage1 * 10000) / 16;
			external_adcs[adc_number].temperatures[1].voltage1 = NTC_table[raw_temp]*0.1 / 2;
	}
}

void BMSH::copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number) {
	for (int adc_number : iota(0, BMSH::EXTERNAL_ADCS)) {
		external_adcs[adc_number].cell_voltages[voltage_number] = voltages[adc_number];
	}
}
