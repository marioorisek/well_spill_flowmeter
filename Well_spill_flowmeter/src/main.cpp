#include <Arduino.h>
#include "cfg.h"
#include "wifi_cfg.h"

float flow_scale = 1;
volatile unsigned int pulse_count = 0;
float flow_rate = 0.0;
unsigned long startTime = 0;


void incrementCounter() {  // IRQ handler
  pulse_count++;
}

void setup() {
  Serial.begin(19200);
  delay(500);
  Serial.println();
  Serial.println("Sensor init..");

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);

# TODO - IRQs doesn't work 
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);

}

void loop() {
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
