#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>

#include "cfg.h"
#include "wifi_cfg.h"

float flow_scale = 750;
volatile unsigned long pulse_count = 0;
volatile unsigned long pulse_count_total = 0;
volatile float flow_total = 0;
float flow_rate;
unsigned long startTime = 0;
String displayText;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

ICACHE_RAM_ATTR
void incrementCounter() {  // IRQ handler
  pulse_count++;
  pulse_count_total++;
}

void drawFlowRate() {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(FONT_LARGE);
  u8g2.setCursor(0,24);
  u8g2.print(flow_total, 1);
  u8g2.setFont(FONT_SMALL);
  u8g2.print(" l");
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
<<<<<<< HEAD
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);
=======
  // attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);
>>>>>>> 57fe89ccbaac8d3b9e01e4ffa11d2306c33c693e

  delay(4000);
  }

void loop() {

  flow_total = pulse_count_total / 450.0;
  drawFlowRate();
  delay(100);

  if ((millis() - startTime) > 1000) {
    flow_rate = flow_scale * pulse_count / (millis() - startTime);
    Serial.print("Current flow: ");
    Serial.print(flow_rate, 1);
    Serial.print(" l / min | Total volume: ");
    Serial.print(pulse_count_total / 450.0, 1);
    Serial.println(" l");

    pulse_count = 0;
    startTime = millis();
<<<<<<< HEAD

=======
  
>>>>>>> 57fe89ccbaac8d3b9e01e4ffa11d2306c33c693e
  }
}
