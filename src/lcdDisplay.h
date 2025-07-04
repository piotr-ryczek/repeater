#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LcdDisplay {
  private:
    LiquidCrystal_I2C* lcd;
    String topRowText = "";
    String bottomRowText = "";

    String padToLength(const String& text, int length = 16);

  public:
    LcdDisplay(LiquidCrystal_I2C* lcd);
    
    void initialize();
    void backlight();
    void noBacklight();
    void print(String topRowText);
    void print(String topRowText, String bottomRowText);
    void clear();
    void clearBottomRow();
    void clearTopRow();
};



#endif