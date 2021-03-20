#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#include "cfg.h"
#include "wifi_cfg.h"

volatile unsigned long pulse_count_total = 0;
volatile unsigned long pulse_count_flow = 0;
volatile float flow_total;
volatile float flow_rate;
volatile long last_pulse_time;
volatile long last_flow_calc_time;
String displayText;
bool discharge_active = false;
bool total_flow_reset_enabled = false;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

WiFiClient client;

ICACHE_RAM_ATTR
void incrementCounter() {  // IRQ handler
  discharge_active = true;
  pulse_count_total++;
  pulse_count_flow++;
  last_pulse_time = millis();
  if (total_flow_reset_enabled) {
    total_flow_reset_enabled = false;
    pulse_count_total = 0;
    pulse_count_flow = 0;
  }
}

int connectWiFi() {

  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {

    WiFi.begin( ssid, password );

    Serial.println("Connecting to WiFi");
    delay(10000);
  }

  Serial.println();
  Serial.print("\nConnected to: " );
  Serial.print(ssid);
  Serial.print(" IP address: ");
  Serial.println(WiFi.localIP());


  ThingSpeak.begin( client );
}

// Use this function if you want to write a single field.
int writeTSData( long TSChannel, unsigned int TSField, float data ) {
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ) {

    Serial.println( String(data) + " written to Thingspeak." );
  }

  return writeSuccess;
}

void calcFlowRate() {
  long current_time = millis();
  long time_delta;
  if (current_time - last_flow_calc_time > FLOW_RATE_WINDOW) {
    time_delta = current_time - last_flow_calc_time;
    flow_rate = (60 * pulse_count_flow / float(time_delta / 1000.0)) / 450;
    pulse_count_flow = 0;
    last_flow_calc_time = millis();
  }
}

void printStatus() {
  Serial.print("Discharge state: ");
  Serial.print(discharge_active);
  Serial.print(" | ");
  if (discharge_active) {
      Serial.print("Curent flow: ");
      Serial.print(flow_rate, 1);
      Serial.print(" l /min | Total flow: ");
      Serial.print(flow_total, 1);
      Serial.print(" l");
  }
    else {
    Serial.print("Last discharged amount: ");
    Serial.print(flow_total, 1);
    Serial.print(" l ");
  }
  Serial.println();
}

void drawFlow() {
  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(FONT_LARGE);
  u8g2.setCursor(0,24);
  if (discharge_active) {
    u8g2.print(flow_rate, 1);
    u8g2.setFont(FONT_SMALL);
    u8g2.print(" l / min");
} else {
  if (flow_total < 1000) {
    u8g2.print(flow_total, 1);
  } else {
    u8g2.print(flow_total, 1);
  }
  u8g2.setFont(FONT_SMALL);
  u8g2.print(" l");
}
  u8g2.sendBuffer();
}

void reportFlow() {
  Serial.println("Discharge finished.");
  writeTSData(channelID, TSField, flow_total);
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  delay(500);

  pinMode(FLOW_SENSOR_PIN, INPUT);
  pinMode(FLOW_SENSOR_POWER, OUTPUT);
  digitalWrite(FLOW_SENSOR_POWER, HIGH);

  u8g2.clearBuffer(); // clear the internal memory
  u8g2.setFont(FONT_LARGE);
  u8g2.drawStr(0,24,"Hello");
  u8g2.sendBuffer();

  // connect to WiFi
  connectWiFi();

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), incrementCounter, FALLING);
  delay(2000);
  }

void loop() {
  if (discharge_active && millis() - last_pulse_time > NO_FLOW_THRESHOLD) {
    discharge_active = false;
    total_flow_reset_enabled = true;
    reportFlow();
  }

  flow_total = pulse_count_total / 450.0;
  calcFlowRate();
  printStatus();
  drawFlow();
  delay(250);

}
