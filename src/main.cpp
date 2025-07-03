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

ButtonHandler mainButton(BUTTON_GPIO);
SteadyView steadyView(STEADY_VIEW_MOSI_GPIO, STEADY_VIEW_CLK_GPIO, STEADY_VIEW_CS_GPIO, &bandIndexMemory, &channelIndexMemory);
VoltageMeter batteryVoltageMeter(BATTERY_VOLTAGE_METER_GPIO, RESISTOR_FIRST_VALUE, RESISTOR_SECOND_VALUE, BATTERY_VOLTAGE_0_REFERENCE, BATTERY_VOLTAGE_100_REFERENCE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
LcdDisplay lcdDisplay(&lcd);

Navigation navigation(&lcdDisplay, &batteryVoltageMeter, &steadyView);

/**
 * Co zrobić:
 * - Ustawianie Mode w SteadyView
 * - Navigation
 * - Długi przycisk: Zmiana Menu
 * - Krótki przycisk: Zmiana wartości (ale z debaunce, dopiero po sekundzie następuję propagacja) (Wyświetl Save przez 3 sekundy i potem znowu wróc do wartości)
 * - po 10s bezruchu wygaś ekran
 * - jeden klik - włącza ponownie ekran
 */

 void handleButtonShortPress() {
    navigation.handleChangeValue();
};

void handleButtonLongPress() {
    navigation.handleChangeMenu();
};

void setup() {
  Serial.begin(115200);

  mainButton.initialize();

  lcdDisplay.initialize();
  mainButton.attachButtonCallbacks(handleButtonShortPress, handleButtonLongPress);

  // if (!EEPROM.begin(EEPROM_SIZE)) {
  //     Serial.println("EEPROM Error");
  //     return;
  // }
  
  // steadyView.initialize();

  delay(200);
}

void loop() {
  mainButton.checkButtonPress();

  navigation.checkIfShouldSleep();
  navigation.handleDisplay();
  steadyView.checkDelayedExecution();

  delay(10);
}