#include <Arduino.h>
#include <buttonHandler.h>

ButtonHandler::ButtonHandler(byte buttonGpio) {
    this->lastButtonState = 1;
    this->buttonGpio = buttonGpio;
    this->shortPressCallback = nullptr;
    this->longPressCallback = nullptr;
    this->longPressThresholdMicros = 1000 * 1000; // 1s
}

void ButtonHandler::initialize() {
    pinMode(buttonGpio, INPUT_PULLUP);
}

void ButtonHandler::attachButtonCallbacks(void (*shortPressCallback)(), void (*longPressCallback)()) {
    this->shortPressCallback = shortPressCallback;
    this->longPressCallback = longPressCallback;
}

void ButtonHandler::checkButtonPress() {
    bool newButtonState = digitalRead(buttonGpio);
    unsigned long currentTime = micros();

    if (lastButtonState != newButtonState) {
        if (newButtonState && currentTime - lastCompareMicros >= 100 * 1000) {
            if (currentTime - lastCompareMicros <= this->longPressThresholdMicros) {
                if (shortPressCallback != nullptr) {
                    shortPressCallback();
                }
            } else {
                if (longPressCallback != nullptr) {
                    longPressCallback();
                }
            }   
        }

        lastCompareMicros = currentTime;
    }

    lastButtonState = newButtonState;
}