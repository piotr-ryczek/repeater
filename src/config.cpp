#include <config.h>

const uint32_t VOLTAGE_REFERENCE = 1100;
const float RESISTOR_FIRST_VALUE = 220000.0;  // 220 000kΩ
const float RESISTOR_SECOND_VALUE = 22000.0;   // 22 000kΩ

const float BATTERY_VOLTAGE_0_REFERENCE = 12.8;
const float BATTERY_VOLTAGE_100_REFERENCE = 16.8;

float lastReadBatteryVoltage = 0;