/*
 * LTC681x.cpp
 *
 *  Created on: Dec 19, 2022
 *      Author: aleja
 */

#include <LTC6811/LTC6811.hpp>

LTC6811::LTC6811() {
	cell_voltages[0] = {0, 0, 0};
	cell_voltages[1] = {0, 0, 0};
	cell_voltages[2] = {0, 0, 0};
	cell_voltages[3] = {0, 0, 0};
	temperatures[0] = {0, 0, 0};
	temperatures[1] = {0, 0, 0};

	peripheral_configuration = configuration();
}

void LTC6811::initialize_batteries() {
	batteries[0] = Battery(&cell_voltages[0], &cell_voltages[1], &temperatures[0].voltage1, &temperatures[0].voltage2);
	batteries[1] = Battery(&cell_voltages[2], &cell_voltages[3], &temperatures[0].voltage3, &temperatures[1].voltage1);
}