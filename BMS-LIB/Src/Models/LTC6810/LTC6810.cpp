#include "LTC6810/LTC6810.hpp"

LTC6810::LTC6810() : cell_voltages{ 0 }, temperatures{ 0 } {
    peripheral_configuration = configuration();
}

void LTC6810::initialize() {
    battery = Battery(&cell_voltages[0], &cell_voltages[1], &temperatures.voltage1, &temperatures.voltage2);
}
