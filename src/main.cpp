#include <Arduino.h>
#include <SPI.h>
#include <steadyView.h>


void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_CS, OUTPUT);

  digitalWrite(PIN_CLK, LOW);
  digitalWrite(PIN_MOSI, HIGH);
  digitalWrite(PIN_CS, HIGH);

  delay(100);
}

void loop() {
  
}