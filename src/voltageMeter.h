#ifndef VOLTAGE_METER_H
#define VOLTAGE_METER_H

#include <Arduino.h>
#include <esp_adc_cal.h>

class VoltageMeter {
  public:
    VoltageMeter(float batteryVoltage0Reference, float batteryVoltage100Reference);

  private:
    float batteryVoltage0Reference;
    float batteryVoltage100Reference;
}; 

#endif