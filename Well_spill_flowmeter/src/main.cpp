#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>

#include "cfg.h"
#include "wifi_cfg.h"

float flow_scale = 1;
volatile unsigned int pulse_count = 0;
float flow_rate = 0.0;
unsigned long startTime = 0;
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);


void incrementCounter() {  // IRQ handler
  pulse_count++;
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  delay(500);

  Serial.println();
  Serial.println("Sensor init..");

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);

  String sOut = "mikrofon";
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(u8g2_font_8x13B_mf);
  u8g2.drawStr(20,20,sOut.c_str());
  u8g2.sendBuffer();

  // attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);

}

void loop() {
  Serial.println(pulse_count);
  delay(200);
  // if ((millis() - startTime) > 1000) {
  //
  //   flow_rate = flow_scale * pulse_count / (millis() - startTime);
  //   Serial.print("Current flow: ");
  //   Serial.print(flow_rate, 1);
  //   Serial.println(" l / min");
  //   pulse_count = 0;
  //   startTime = millis();
  //
  // }
}
