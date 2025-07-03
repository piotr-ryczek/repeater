#ifndef NAVIGATION_H
#define NAVIGATION H

#include <lcdDisplay.h>
#include <voltageMeter.h>
#include <steadyView.h>

enum class AppMainStateEnum { Sleep, Awaken };
enum class MainMenuEnum {
    FREQUENCY,
    MODE,
    VOLTAGE
};

extern const MainMenuEnum menu[4];

class Navigation {
  private:
    unsigned long lastActionMicros;
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
};

#endif