#include <lcdDisplay.h>

LcdDisplay::LcdDisplay(LiquidCrystal_I2C* lcd): lcd(lcd) {}

void LcdDisplay::initialize() {
  this->lcd->init();
}

void LcdDisplay::backlight() {
  this->lcd->backlight();
}

void LcdDisplay::noBacklight() {
  this->lcd->noBacklight();
}

void LcdDisplay::clearTopRow() {
  this->topRowText = "";
  this->lcd->setCursor(0, 0);
  this->lcd->print("                                ");
}

void LcdDisplay::clearBottomRow() {
  this->bottomRowText = "";
  this->lcd->setCursor(0, 1);
  this->lcd->print("                                ");
}

void LcdDisplay::print(String topRowText) {
  if (topRowText == this->topRowText && this->bottomRowText == "") {
    return;
  }


  this->lcd->setCursor(0,0);
  this->lcd->print(this->padToLength(topRowText));
  this->clearBottomRow();

  this->topRowText = topRowText;
  this->bottomRowText = "";
}

void LcdDisplay::print(String topRowText, String bottomRowText) {
  if (topRowText == this->topRowText && bottomRowText == this->bottomRowText) {
    return;
  }

  this->lcd->setCursor(0,0);
  this->lcd->print(this->padToLength(topRowText));
  this->lcd->setCursor(0,1);
  this->lcd->print(this->padToLength(bottomRowText));

  this->topRowText = topRowText;
  this->bottomRowText = bottomRowText;
}

void LcdDisplay::clear() {
  this->topRowText = "";
  this->bottomRowText = "";
  this->lcd->clear();
}

String LcdDisplay::padToLength(const String& text, int length) {
  String result = text;

  while (result.length() < length) {
    result += " ";
  }
  
  return result;
}