#include <navigation.h>
#include <algorithm>


const MainMenuEnum menuList[4] = { MainMenuEnum::BAND, MainMenuEnum::CHANNEL, MainMenuEnum::MODE, MainMenuEnum::VOLTAGE };

Navigation::Navigation(LcdDisplay* lcdDisplay, VoltageMeter* batteryVoltageMeter, SteadyView* steadyView): lcdDisplay(lcdDisplay), batteryVoltageMeter(batteryVoltageMeter), steadyView(steadyView) {
  this->lastActionMicros = 0;
  this->sleepThresholdMicros = 1000 * 1000 * 10; // 10s
  this->currentMainMenu = MainMenuEnum::BAND;
  this->appMainState = AppMainStateEnum::SLEEP;
};

void Navigation::checkIfShouldSleep() {
  auto now = micros();
  if (this->appMainState == AppMainStateEnum::AWAKEN && now - this->lastActionMicros > this->sleepThresholdMicros) {
    this->currentMainMenu = MainMenuEnum::BAND;
    this->appMainState = AppMainStateEnum::SLEEP;
    this->lastActionMicros = now;

    this->lcdDisplay->noBacklight();
  }
}

void Navigation::handleChangeMenu() {
  auto it = find(begin(menuList), end(menuList), this->currentMainMenu);

  if (it == end(menuList)) {
    throw runtime_error("Invalid menu");
  }

  int index = distance(begin(menuList), it);

  int newIndex;
  if (index == sizeof(menuList)) {
    newIndex = 0;
  } else {
    newIndex++;
  }

  this->currentMainMenu = menuList[newIndex];
  this->lastActionMicros = micros();
}

void Navigation::handleChangeValue() {
  switch (this->appMainState) {
    case AppMainStateEnum::SLEEP: {
      this->appMainState = AppMainStateEnum::AWAKEN;
      this->lcdDisplay->backlight();

      break;
    }

    case AppMainStateEnum::AWAKEN: {
      switch (this->currentMainMenu) {
        case MainMenuEnum::BAND: {
          this->steadyView->increaseBandIndex();
          break;
        }

        case MainMenuEnum::CHANNEL: {
          this->steadyView->increaseChannelIndex();
          break;
        }

        case MainMenuEnum::MODE: {
          // TODO:
          break;
        }

        case MainMenuEnum::VOLTAGE: {
          // Nothing
          break;
        }
        
      }

      break;
    }
  }
}

void Navigation::handleDisplay() {

}