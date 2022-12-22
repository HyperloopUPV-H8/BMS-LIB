/*
 * LTC681x.cpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#include "LTC6811/LTC6811.hpp"


battery::battery(voltage_register_group& voltage_register1, voltage_register_group& voltage_register2, uint16_t& temp1, uint16_t& temp2) {
	cell1 = voltage_register1.voltage1;
	cell2 = voltage_register1.voltage2;
	cell3 = voltage_register1.voltage3;
	cell4 = voltage_register2.voltage1;
	cell5 = voltage_register2.voltage2;
	cell6 = voltage_register2.voltage3;
	temperature1 = temp1;
	temperature2 = temp2;
}

LTC6811::LTC6811() : cell_voltages({ 0 }), temperatures({ 0 }){
	battery1 = battery(cell_voltages[0], cell_voltages[1], temperatures[0].voltage1, temperatures[0].voltage2);
	battery2 = battery(cell_voltages[2], cell_voltages[3], temperatures[0].voltage3, temperatures[1].voltage1);
}
