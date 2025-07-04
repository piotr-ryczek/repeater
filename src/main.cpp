#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

#include <steadyView.h>
#include <gpios.h>
#include <memoryData.h>
#include <config.h>
#include <voltageMeter.h>
#include <buttonHandler.h>
#include <lcdDisplay.h>
#include <navigation.h>

#define EEPROM_SIZE 512

// Task Handles
TaskHandle_t BatteryVoltageTask;

ButtonHandler mainButton(BUTTON_GPIO);
SteadyView steadyView(STEADY_VIEW_MOSI_GPIO, STEADY_VIEW_CLK_GPIO, STEADY_VIEW_CS_GPIO, &bandIndexMemory, &channelIndexMemory, &modeIndexMemory);
VoltageMeter batteryVoltageMeter(BATTERY_VOLTAGE_METER_GPIO, RESISTOR_FIRST_VALUE, RESISTOR_SECOND_VALUE, BATTERY_VOLTAGE_0_REFERENCE, BATTERY_VOLTAGE_100_REFERENCE, lastReadBatteryVoltage);

LiquidCrystal_I2C lcd(0x27, 16, 2);
LcdDisplay lcdDisplay(&lcd);

Navigation navigation(&lcdDisplay, &batteryVoltageMeter, &steadyView);

void batteryVoltageTask(void *param) {
  while (true) {
      batteryVoltageMeter.getVoltage();

      vTaskDelay(10000 / portTICK_PERIOD_MS); // Once per 10s
    }
}

void handleButtonShortPress() {
    navigation.handleChangeValue();
};

void handleButtonLongPress() {
    navigation.handleChangeMenu();
};

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.println("Initialization");

  mainButton.initialize();
  lcdDisplay.initialize();
  mainButton.attachButtonCallbacks(handleButtonShortPress, handleButtonLongPress);
  batteryVoltageMeter.initialize();

  if (!EEPROM.begin(EEPROM_SIZE)) {
      Serial.println("EEPROM Error");
      return;
  }
  
  steadyView.initialize();

  delay(200);

  // Tasks

  xTaskCreate(batteryVoltageTask, "BatteryVoltageTask", 2048, NULL, 1, &BatteryVoltageTask);
}

void loop() {
  mainButton.checkButtonPress();

  navigation.checkIfShouldSleep();
  navigation.handleDisplay();
  steadyView.checkDelayedExecution();

  delay(10);
}