/*
 * Battery.cpp
 *
 *  Created on: 22 dic. 2022
 *      Author: aleja
 */

#include "Battery/Battery.hpp"

Battery::Battery(voltage_register_group& voltage_register1, voltage_register_group& voltage_register2, uint16_t& temp1, uint16_t& temp2) {
	cell1 = voltage_register1.voltage1;
	cell2 = voltage_register1.voltage2;
	cell3 = voltage_register1.voltage3;
	cell4 = voltage_register2.voltage1;
	cell5 = voltage_register2.voltage2;
	cell6 = voltage_register2.voltage3;
	temperature1 = temp1;
	temperature2 = temp2;
}
