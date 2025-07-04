#ifndef VOLTAGE_METER_H
#define VOLTAGE_METER_H

#include <Arduino.h>
#include <esp_adc_cal.h>
#include <config.h>

class VoltageMeter {
  private:
    esp_adc_cal_characteristics_t *adc_chars;
    byte batteryVoltageMeterPin;
    float resistorFirstValue;
    float resistorSecondValue;
    float batteryVoltage0Reference;
    float batteryVoltage100Reference;
    float calculatePercentage(float batteryVoltage);
    float &lastReadBatteryVoltage;

  public:
    VoltageMeter(byte batteryVoltageMeterPin, float resistorFirstValue, float resistorSecondValue,  float batteryVoltage0Reference, float batteryVoltage100Reference, float &lastReadBatteryVoltage);
    void initialize();
    float getVoltage();
    String getBatteryVoltageMessage();
}; 

#endif