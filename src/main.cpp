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

// int r7index = 38;
// int otherIndex = 16;

// int tempIndex = r7index;

int tempIndex = 0;

void loop() {
  // if (tempIndex == r7index) {
  //   tempIndex = otherIndex;
  // } else {
  //   tempIndex = r7index;
  // }

  tempIndex++;
  if (tempIndex >= 48) {
    tempIndex = 0;
  }

  Serial.print("tempIndex: ");
  Serial.println(tempIndex);
  SendIndexCmd(tempIndex);
  delay(1000);
}