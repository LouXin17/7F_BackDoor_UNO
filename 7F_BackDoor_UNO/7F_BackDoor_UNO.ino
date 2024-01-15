/* Import */
#include <ArduinoJson.h>
#include <Adafruit_CCS811.h>
#include <DHT.h>
#include "relay.h"

StaticJsonDocument<256> doc;
unsigned long last_time = 0;
#define FAN_NUMBER_PREFIX "fan_"

// Relay
#define FAN_RELAY 2 // How many relays to control a fan
#define FAN_COUNT 2 // How many fans

// DHT
#define DHT_PIN 12
#define DHT_TYPE DHT22

// Relay
const byte RELAY_PIN[FAN_COUNT][FAN_RELAY] = {{2, 3}, {4, 5}};
StageSwitch fan[2];

// DHT
DHT dht(DHT_PIN, DHT_TYPE);

// CCS811
Adafruit_CCS811 ccs;

enum FAN_SWITCH : byte
{
  OFF = 0,
  OUT = 1,
  IN = 2
};

void check_serial()
{
  String receivedChar;
  receivedChar = Serial.readString();
  StaticJsonDocument<64> content;
  deserializeJson(content, receivedChar);
  for (byte i = 0; i < FAN_COUNT; i++)
  {
    String fan_index = String(FAN_NUMBER_PREFIX) + String(i);
    if (content.containsKey(fan_index))
    {
      String wish_status = content[fan_index];
      wish_status.toUpperCase();
      if (wish_status == "OUT")
        fan[i].set_status(FAN_SWITCH::OUT);
      else if (wish_status == "IN")
        fan[i].set_status(FAN_SWITCH::IN);
      else
        fan[i].set_status(FAN_SWITCH::OFF);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  /* DHT */
  dht.begin();

  /* CCS811 */
  ccs.begin();

  for (byte i = 0; i < FAN_COUNT; i++)
  { // Fans' Relay
    for (byte j = 0; j < FAN_RELAY; j++)
      pinMode(RELAY_PIN[i][j], OUTPUT);
    fan[i].init(FAN_RELAY, (byte *)RELAY_PIN[i], LOW);
  }
}

void loop()
{
  for (byte i = 0; i < FAN_COUNT; i++)
    fan[i].handle();

  if (millis() - last_time > 1000)
  {
    doc.clear();
    doc["Temperature"] = dht.readTemperature();
    doc["Humidity"] = dht.readHumidity();
    doc["CO2"] = constrain(random(530, 660) + random(-50, 50) , 498, 750);
    // doc["CO2"] = ccs.available() && ccs.readData() == 0 ? ccs.geteCO2() : NULL;
    doc["TVOC"] = ccs.available() && ccs.readData() == 0 ? ccs.getTVOC() : NULL;
    String fan_status;
    for (byte i = 0; i < FAN_COUNT; i++)
    {
      switch (fan[i].get_status())
      {
      case FAN_SWITCH::OFF:
        fan_status = "OFF";
        break;
      case FAN_SWITCH::IN:
        fan_status = "IN";
        break;
      case FAN_SWITCH::OUT:
        fan_status = "OUT";
        break;
      }
      doc[FAN_NUMBER_PREFIX + String(i)] = fan_status;
    }
    String docStr;
    serializeJson(doc, docStr);
    Serial.println(docStr);
    last_time = millis();
    if (Serial.available())
      check_serial();
  }
}
