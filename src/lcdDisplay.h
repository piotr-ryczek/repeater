#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LcdDisplay {
  private:
    LiquidCrystal_I2C* lcd;

  public:
    LcdDisplay(LiquidCrystal_I2C* lcd)
    void initialize();
    void backlight();
    void noBacklight();
    void print(String topRowText);
    void print(String topRowText, String bottomRowText);
    void clear();
};



#endif