#include <navigation.h>
#include <algorithm>

const MainMenuEnum menuList[4] = { MainMenuEnum::BAND, MainMenuEnum::CHANNEL, MainMenuEnum::MODE, MainMenuEnum::VOLTAGE };

Navigation::Navigation(LcdDisplay* lcdDisplay, VoltageMeter* batteryVoltageMeter, SteadyView* steadyView): lcdDisplay(lcdDisplay), batteryVoltageMeter(batteryVoltageMeter), steadyView(steadyView) {
  this->lastActionMicros = 0;
  this->sleepThresholdMicros = 1000 * 1000 * 10; // 10s
  this->lastDisplayBlink = 0;
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

    Serial.println("Sleep");
  }
}

void Navigation::handleChangeMenu() {
  this->lastActionMicros = micros();

  auto it = find(begin(menuList), end(menuList), this->currentMainMenu);

  if (it == end(menuList)) {
    throw runtime_error("Invalid menu");
  }

  int index = distance(begin(menuList), it);

  int newIndex = index;;
  int menuListSize = sizeof(menuList) / sizeof(menuList[0]);
  
  if (index == menuListSize - 1) {
    newIndex = 0;
  } else {
    newIndex++;
  }

  this->currentMainMenu = menuList[newIndex];
}

void Navigation::handleChangeValue() {
  this->lastActionMicros = micros();

  switch (this->appMainState) {
    case AppMainStateEnum::SLEEP: {
      this->appMainState = AppMainStateEnum::AWAKEN;
      this->lcdDisplay->backlight();

      Serial.println("Awake");

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
          this->steadyView->increaseModeIndex();
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
  if (this->appMainState == AppMainStateEnum::SLEEP) {
    return;
  }

  unsigned long now = micros();
  bool ifShouldBlink = now - lastDisplayBlink > 1000 * 1000; // 1s

  switch (this->currentMainMenu) {
    case MainMenuEnum::BAND: {
      auto [band, channel, frequency] = this->steadyView->getBandAndChannelAndFrequency();
      auto mode =  this->steadyView->getMode();

      String topRowText = ifShouldBlink 
        ? String(" ") + String(channel) + " " + String(mode)
        : String(band) + String (channel) + " " + String(mode);


      this->lcdDisplay->print(topRowText, String(frequency));

      break;
    }

    case MainMenuEnum::CHANNEL: {
      auto [band, channel, frequency] = this->steadyView->getBandAndChannelAndFrequency();
      auto mode =  this->steadyView->getMode();

      String topRowText = ifShouldBlink 
        ? String(band) + String(" ") + " " + String(mode)
        : String(band) + String (channel) + " " + String(mode);

      this->lcdDisplay->print(topRowText, String(frequency));
      
      break;
    }

    case MainMenuEnum::MODE: {
      auto [band, channel, frequency] = this->steadyView->getBandAndChannelAndFrequency();
      auto mode =  this->steadyView->getMode();

      String topRowText = ifShouldBlink 
        ? String(band) + String (channel) + " " + String(" ")
        : String(band) + String (channel) + " " + String(mode);

      this->lcdDisplay->print(topRowText, String(frequency));
      break;
    }

    case MainMenuEnum::VOLTAGE: {
      this->lcdDisplay->print(batteryVoltageMeter->getBatteryVoltageMessage());
      break;
    }
  }

  if (ifShouldBlink && now - lastDisplayBlink > 1000 * 1000 * 1.5) {
    lastDisplayBlink = now;
  }
}