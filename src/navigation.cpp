#include <navigation.h>

const MainMenuEnum menu[4] = { MainMenuEnum::FREQUENCY, MainMenuEnum::MODE, MainMenuEnum::VOLTAGE };

Navigation::Navigation(LcdDisplay* lcdDisplay, VoltageMeter* batteryVoltageMeter, SteadyView* steadyView): lcdDisplay(lcdDisplay), batteryVoltageMeter(batteryVoltageMeter), steadyView(steadyView) {
  this->lastActionMicros = 0;
  this->currentMainMenu = MainMenuEnum::FREQUENCY;
  this->appMainState = AppMainStateEnum::Sleep;
};

void Navigation::checkIfShouldSleep() {
  
}

