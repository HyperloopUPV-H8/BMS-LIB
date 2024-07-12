/*
 * BMSA.cpp
 *
 *  Created on: 16 ene. 2023
 *      Author: aleja
 */

#include "BMSA/BMSA.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

BMSA::BMSA(SPI::Peripheral& spi_peripheral) {
	spi_instance = SPI::inscribe(spi_peripheral);
	external_adc = LTC6810();
}

span<BMS::COMMAND> BMSA::get_cell_voltage_registers() {
	return span(cell_voltage_registers.begin(), cell_voltage_registers.end()-2);
}

void BMSA::copy_voltages_to_external_adcs(array<voltage_register_group, BMS::EXTERNAL_ADCS> voltages, uint8_t voltage_number) {
	for (int adc_number : iota(0, BMS::EXTERNAL_ADCS)) {
		external_adc.cell_voltages[voltage_number] = voltages[adc_number];
	}
}

void BMSA::parse_configuration_data_stream(span<uint8_t> data_stream) {
	uint8_t offset = 0;
	for (bitset<8> data_register : external_adc.peripheral_configuration.register_group) {
		data_stream[offset] = (uint8_t)data_register.to_ulong(); //TODO: Refactor this, is horrible.
		offset++;
	}
}

void BMSA::check_adcs() {
	Battery battery = external_adc.battery;

	if(not battery.needs_balance()) {
		return;
	}

	for(int i : iota(0, Battery::CELLS)) {
		float& min_cell = battery.minimum_cell;
		float& curr_cell = *battery.cells[i];


		float soc_difference = SOC::calculate(curr_cell) - SOC::calculate(min_cell);
		LTC681X::configuration config = external_adc.peripheral_configuration;
		if (soc_difference > SOC::MAX_DIFFERENCE) {
			config.set_cell_discharging(i, true);
		}

		else if (soc_difference <= SOC::MAX_DIFFERENCE/2){
			config.set_cell_discharging(i, false);
		}
	}
}

void BMSA::copy_internal_temperature(array<uint16_t, BMS::EXTERNAL_ADCS> temperatures) {
	external_adc.internal_temperature = (float)temperatures[0] / 75 - 273;
}

void BMSA::initialize() {
	external_adc.initialize();
}

void BMSA::deactivate_cell_discharging() {
	for (int i : iota(0, 2*Battery::CELLS)) {
		external_adc.peripheral_configuration.set_cell_discharging(i, false);
	}
}

void BMSA::parse_temperatures(array<voltage_register_group, BMSA::EXTERNAL_ADCS> temperatures_register1, array<voltage_register_group, BMSA::EXTERNAL_ADCS> temperatures_register2) {
	float raw_temp = temperatures_register1[0].voltage2 * 10000;
	float beta_value = 3976;
	float nominal_temperature = 25;
	float resistance = 10000;
	float adc_res = pow(2, 16);
	
	// if(raw_temp == 65535) external_adc.temperatures.voltage1 = -40; 
	// else external_adc.temperatures.voltage1 = ((1/((log(((resistance * raw_temp) / (adc_res - raw_temp))/resistance)/beta_value) + (1 / (273.15 + nominal_temperature)))) - 273.15) / 2;

	/*raw_temp = temperatures_register1[0].voltage2 * 10000;
	if(raw_temp == 65535) external_adc.temperatures.voltage2 = -40; 
	else external_adc.temperatures.voltage2 = ((1/((log(((resistance * raw_temp) / (adc_res - raw_temp))/resistance)/beta_value) + (1 / (273.15 + nominal_temperature)))) - 273.15) / 2;

	external_adc.temperatures.voltage1 = external_adc.temperatures.voltage2+0.21+ (std::rand()%100)/100;*/
}
