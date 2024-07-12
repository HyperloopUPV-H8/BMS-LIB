#include "LTC6810/LTC6810.hpp"

LTC6810::LTC6810(){
    cell_voltages[0] = {0, 0, 0};
    cell_voltages[1] = {0, 0, 0};
    temperatures1 = {0, 0, 0};
    temperatures2 = {0, 0, 0};
    peripheral_configuration = configuration();
} 


void LTC6810::initialize() {
    battery = Battery(&cell_voltages[0], &cell_voltages[1], &temperatures2.voltage1, &temperatures2.voltage2);
}
