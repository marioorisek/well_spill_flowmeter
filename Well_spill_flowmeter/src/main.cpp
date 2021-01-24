#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>

#include "cfg.h"
#include "wifi_cfg.h"

float flow_scale = 1;
volatile unsigned int pulse_count = 0;
float flow_rate = 1.754;
unsigned long startTime = 0;
String displayText;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

void incrementCounter() {  // IRQ handler
  pulse_count++;
}

void drawFlowRate() {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(FONT_LARGE);
  u8g2.setCursor(0,24);
  u8g2.print(flow_rate, 1);
  u8g2.setFont(FONT_SMALL);
  u8g2.print(" l/min");
  u8g2.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  delay(500);

  Serial.println();
  Serial.println("Sensor init..");

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);


  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(FONT_LARGE);
  u8g2.drawStr(0,24,"Hello");
  u8g2.sendBuffer();
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);

  delay(4000);
  interrupts();
}

void loop() {

  drawFlowRate();
  delay(100);

  if ((millis() - startTime) > 1000) {
    noInterrupts();
    flow_rate = flow_scale * pulse_count / (millis() - startTime);
    Serial.print("Current flow: ");
    Serial.print(flow_rate, 1);
    Serial.println(" l / min");
    pulse_count = 0;
    startTime = millis();
    interrupts();
  }
}
