/*
 * soc.hpp
 *
 *  Created on: 6 feb. 2023
 *      Author: aleja
 */

#pragma once

#include "ST-LIB.hpp"
#include "Cell/Cell.hpp"

class SOC {
private:
	static constexpr uint16_t voltage_to_soc[901] = {0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 38, 41, 44, 47, 50, 53, 56, 59, 62, 66, 69, 72, 75, 78, 81, 84, 88, 91, 94, 97, 100, 104, 108, 112, 117, 121, 125, 129, 133, 138, 142, 146, 150, 154, 158, 162, 167, 171, 175, 179, 183, 188, 192, 196, 200, 204, 209, 213, 217, 222, 226, 230, 235, 239, 243, 248, 252, 257, 261, 265, 270, 274, 278, 283, 287, 291, 296, 300, 304, 308, 312, 317, 321, 325, 329, 333, 338, 342, 346, 350, 354, 358, 362, 367, 371, 375, 379, 383, 388, 392, 396, 400, 404, 409, 413, 417, 422, 426, 430, 435, 439, 443, 448, 452, 457, 461, 465, 470, 474, 478, 483, 487, 491, 496, 500, 504, 508, 512, 517, 521, 525, 529, 533, 538, 542, 546, 550, 554, 558, 562, 567, 571, 575, 579, 583, 588, 592, 596, 600, 604, 609, 613, 617, 622, 626, 630, 635, 639, 643, 648, 652, 657, 661, 665, 670, 674, 678, 683, 687, 691, 696, 700, 704, 708, 712, 717, 721, 725, 729, 733, 738, 742, 746, 750, 754, 758, 762, 767, 771, 775, 779, 783, 788, 792, 796, 800, 804, 809, 813, 817, 822, 826, 830, 835, 839, 843, 848, 852, 857, 861, 865, 870, 874, 878, 883, 887, 891, 896, 900, 904, 908, 912, 917, 921, 925, 929, 933, 938, 942, 946, 950, 954, 958, 962, 967, 971, 975, 979, 983, 988, 992, 996, 1000, 1004, 1009, 1013, 1017, 1022, 1026, 1030, 1035, 1039, 1043, 1048, 1052, 1057, 1061, 1065, 1070, 1074, 1078, 1083, 1087, 1091, 1096, 1100, 1104, 1108, 1112, 1117, 1121, 1125, 1129, 1133, 1138, 1142, 1146, 1150, 1154, 1158, 1162, 1167, 1171, 1175, 1179, 1183, 1188, 1192, 1196, 1200, 1204, 1209, 1213, 1217, 1222, 1226, 1230, 1235, 1239, 1243, 1248, 1252, 1257, 1261, 1265, 1270, 1274, 1278, 1283, 1287, 1291, 1296, 1300, 1304, 1308, 1312, 1317, 1321, 1325, 1329, 1333, 1338, 1342, 1346, 1350, 1354, 1358, 1362, 1367, 1371, 1375, 1379, 1383, 1388, 1392, 1396, 1400, 1404, 1409, 1413, 1417, 1422, 1426, 1430, 1435, 1439, 1443, 1448, 1452, 1457, 1461, 1465, 1470, 1474, 1478, 1483, 1487, 1491, 1496, 1500, 1504, 1508, 1512, 1517, 1521, 1525, 1529, 1533, 1538, 1542, 1546, 1550, 1554, 1558, 1562, 1567, 1571, 1575, 1579, 1583, 1588, 1592, 1596, 1600, 1604, 1609, 1613, 1617, 1622, 1626, 1630, 1635, 1639, 1643, 1648, 1652, 1657, 1661, 1665, 1670, 1674, 1678, 1683, 1687, 1691, 1696, 1700, 1704, 1708, 1712, 1717, 1721, 1725, 1729, 1733, 1738, 1742, 1746, 1750, 1754, 1758, 1762, 1767, 1771, 1775, 1779, 1783, 1788, 1792, 1796, 1800, 1820, 1840, 1860, 1880, 1900, 1920, 1940, 1960, 1980, 2000, 2033, 2067, 2100, 2133, 2167, 2200, 2220, 2240, 2260, 2280, 2300, 2314, 2329, 2343, 2357, 2371, 2386, 2400, 2414, 2429, 2443, 2457, 2471, 2486, 2500, 2520, 2540, 2560, 2580, 2600, 2620, 2640, 2660, 2680, 2700, 2720, 2740, 2760, 2780, 2800, 2820, 2840, 2860, 2880, 2900, 2920, 2940, 2960, 2980, 3000, 3033, 3067, 3100, 3150, 3200, 3233, 3267, 3300, 3350, 3400, 3433, 3467, 3500, 3533, 3567, 3600, 3625, 3650, 3675, 3700, 3733, 3767, 3800, 3833, 3867, 3900, 3925, 3950, 3975, 4000, 4033, 4067, 4100, 4150, 4200, 4233, 4267, 4300, 4350, 4400, 4420, 4440, 4460, 4480, 4500, 4600, 4625, 4650, 4675, 4700, 4733, 4767, 4800, 4833, 4867, 4900, 4925, 4950, 4975, 5000, 5020, 5040, 5060, 5080, 5100, 5120, 5140, 5160, 5180, 5200, 5220, 5240, 5260, 5280, 5300, 5320, 5340, 5360, 5380, 5400, 5420, 5440, 5460, 5480, 5500, 5520, 5540, 5560, 5580, 5600, 5620, 5640, 5660, 5680, 5700, 5720, 5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, 5900, 5920, 5940, 5960, 5980, 6000, 6020, 6040, 6060, 6080, 6100, 6120, 6140, 6160, 6180, 6200, 6220, 6240, 6260, 6280, 6300, 6320, 6340, 6360, 6380, 6400, 6420, 6440, 6460, 6480, 6500, 6520, 6540, 6560, 6580, 6600, 6620, 6640, 6660, 6680, 6700, 6720, 6740, 6760, 6780, 6800, 6820, 6840, 6860, 6880, 6900, 6920, 6940, 6960, 6980, 7000, 7020, 7040, 7060, 7080, 7100, 7120, 7140, 7160, 7180, 7200, 7220, 7240, 7260, 7280, 7300, 7320, 7340, 7360, 7380, 7400, 7420, 7440, 7460, 7480, 7500, 7520, 7540, 7560, 7580, 7600, 7620, 7640, 7660, 7680, 7700, 7720, 7740, 7760, 7780, 7800, 7820, 7840, 7860, 7880, 7900, 7920, 7940, 7960, 7980, 8000, 8010, 8020, 8030, 8040, 8050, 8060, 8070, 8080, 8090, 8100, 8112, 8125, 8138, 8150, 8162, 8175, 8188, 8200, 8210, 8220, 8230, 8240, 8250, 8260, 8270, 8280, 8290, 8300, 8310, 8320, 8330, 8340, 8350, 8360, 8370, 8380, 8390, 8400, 8410, 8420, 8430, 8440, 8450, 8460, 8470, 8480, 8490, 8500, 8508, 8517, 8525, 8533, 8542, 8550, 8558, 8567, 8575, 8583, 8592, 8600, 8620, 8640, 8660, 8680, 8700, 8712, 8725, 8738, 8750, 8762, 8775, 8788, 8800, 8810, 8820, 8830, 8840, 8850, 8860, 8870, 8880, 8890, 8900, 8908, 8915, 8923, 8931, 8938, 8946, 8954, 8962, 8969, 8977, 8985, 8992, 9000, 9007, 9014, 9021, 9029, 9036, 9043, 9050, 9057, 9064, 9071, 9079, 9086, 9093, 9100, 9109, 9118, 9127, 9136, 9145, 9155, 9164, 9173, 9182, 9191, 9200, 9211, 9222, 9233, 9244, 9256, 9267, 9278, 9289, 9300, 9312, 9325, 9338, 9350, 9362, 9375, 9388, 9400, 9412, 9425, 9438, 9450, 9462, 9475, 9488, 9500, 9508, 9517, 9525, 9533, 9542, 9550, 9558, 9567, 9575, 9583, 9592, 9600, 9608, 9617, 9625, 9633, 9642, 9650, 9658, 9667, 9675, 9683, 9692, 9700, 9710, 9720, 9730, 9740, 9750, 9760, 9770, 9780, 9790, 9800, 9812, 9825, 9838, 9850, 9862, 9875, 9888, 9900, 9910, 9920, 9930, 9940, 9950, 9960, 9970, 9980, 9990, 10000};

public:
	static const uint8_t MAX_DIFFERENCE = 100;
	static float calculate(float raw_voltage);
};
