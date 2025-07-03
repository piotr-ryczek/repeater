#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

class ButtonHandler {
    private:
        bool lastButtonState;
        unsigned long lastCompareMicros; // Microseconds
        unsigned long longPressThresholdMicros; // Microseconds
        byte buttonGpio;
        void (*shortPressCallback)();   
        void (*longPressCallback)();   

    public:
        ButtonHandler(byte buttonGpio);
        void initialize();

        void attachButtonCallbacks(void (*shortPressCallback)(), void (*longPressCallback)());

        void checkButtonPress();
};

#endif