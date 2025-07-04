#include <voltageMeter.h>

VoltageMeter::VoltageMeter(byte batteryVoltageMeterPin, float resistorFirstValue, float resistorSecondValue, float batteryVoltage0Reference, float batteryVoltage100Reference, float &lastReadBatteryVoltage) : batteryVoltageMeterPin(batteryVoltageMeterPin), resistorFirstValue(resistorFirstValue), resistorSecondValue(resistorSecondValue), batteryVoltage0Reference(batteryVoltage0Reference), batteryVoltage100Reference(batteryVoltage100Reference), lastReadBatteryVoltage(lastReadBatteryVoltage) {}

void VoltageMeter::initialize() {
    adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    if (adc_chars == NULL) {
        Serial.println("ERROR: Memory allocation failed");
        return;
    }

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, VOLTAGE_REFERENCE, adc_chars);
}

float VoltageMeter::calculatePercentage(float batteryVoltage) {
    float batteryPercentage = (batteryVoltage - batteryVoltage0Reference) / (batteryVoltage100Reference - batteryVoltage0Reference) * 100;

    if (batteryPercentage < 0) {
        batteryPercentage = 0;
    } else if (batteryPercentage > 100) {
        batteryPercentage = 100;
    }

    return batteryPercentage;
}

float VoltageMeter::getVoltage() {
    int rawVoltageValue = analogRead(this->batteryVoltageMeterPin);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(rawVoltageValue, adc_chars);
    float batteryVoltageInmV = voltage * (resistorFirstValue + resistorSecondValue) / resistorSecondValue;
    float batteryVoltageInV = batteryVoltageInmV / 1000; // mV to V

    this->lastReadBatteryVoltage = batteryVoltageInV;

    return batteryVoltageInV; // mV to V
}

String VoltageMeter::getBatteryVoltageMessage() {
  return String(this->lastReadBatteryVoltage) + "V (" + String(calculatePercentage(this->lastReadBatteryVoltage)) + "%)";
}
  