#ifndef NAVIGATION_H
#define NAVIGATION H

#include <lcdDisplay.h>
#include <voltageMeter.h>
#include <steadyView.h>
#include <functional>

using namespace std;

enum class AppMainStateEnum { SLEEP, AWAKEN };
enum class MainMenuEnum {
    BAND,
    CHANNEL,
    MODE,
    VOLTAGE
};

extern const MainMenuEnum menuList[4];

class Navigation {
  private:
    unsigned long lastActionMicros;
    unsigned long sleepThresholdMicros;

    MainMenuEnum currentMainMenu;
    AppMainStateEnum appMainState;

    LcdDisplay* lcdDisplay;
    VoltageMeter* batteryVoltageMeter;
    SteadyView* steadyView;

  public:
    Navigation(LcdDisplay* lcdDisplay, VoltageMeter* batteryVoltageMeter, SteadyView* steadyView);
    void handleChangeValue();
    void handleChangeMenu();
    void checkIfShouldSleep();
    void handleDisplay();
};

#endif