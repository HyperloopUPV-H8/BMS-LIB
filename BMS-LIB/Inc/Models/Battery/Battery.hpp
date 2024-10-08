/*
 * Battery.hpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "SOC/SOC.hpp"

using std::views::iota;

struct voltage_register_group {
	float voltage1;
	float voltage2;
	float voltage3;
};

class Battery {
public:
	static constexpr int CELLS = 6;
	static const int MIN_TEMPERATURE = 0;
	static const int MAX_TEMPERATURE = 60;
	static constexpr float MAX_DISBALANCE = 0.2;

	float* cells[CELLS];
	float minimum_cell;
	float maximum_cell;
	float total_voltage;
	float disbalance;
	bool is_connected;
	

	float* temperature1;
	float* temperature2;
	float filtered_temp;

	bool is_balancing = false;

	float SOC;

	Battery() = default;
	Battery(voltage_register_group* cell_register1, voltage_register_group* cell_register2, float* temperature1, float* temperature2);

	void update_data();
	bool needs_balance();
	bool is_adc_connected()const{
		if((total_voltage < 18  && total_voltage != 0)|| total_voltage > 26){
			return false;
		}
		return true;
	}
	bool are_temps_connected()const{return *temperature1 > 0;}
private:
	float get_charging_maximum_voltage();
	MovingAverage<50> temp_filter;
	static const uint16_t soc[901];
};
