#include <Arduino.h>
#include "relay.h"


enum class RELAY_LOW_TRIGGER : int {
  RELAY_ON=LOW,
  RELAY_OFF=HIGH
};

enum class RELAY_HIGH_TRIGGER : int {
  RELAY_ON=HIGH,
  RELAY_OFF=LOW
};


RelaySwitch::RelaySwitch(void) {
  ;
};

void RelaySwitch::init(byte relay_pin, byte trigger_mode) {
  this->pin = relay_pin;
  this->trigger_mode = trigger_mode;
  this->turn_off();
};

bool RelaySwitch::get_status() {
  return this->status;
}

void RelaySwitch::switch_relay() {
  switch (this->trigger_mode) {
    case HIGH:
      switch (this->status) {
        case true:
          digitalWrite(this->pin, (uint8_t)RELAY_HIGH_TRIGGER::RELAY_ON);
          break;
        case false:
          digitalWrite(this->pin, (uint8_t)RELAY_HIGH_TRIGGER::RELAY_OFF);
          break;
      }
      break;
    case LOW:
      switch (this->status) {
        case true:
          digitalWrite(this->pin, (uint8_t)RELAY_LOW_TRIGGER::RELAY_ON);
          break;
        case false:
          digitalWrite(this->pin, (uint8_t)RELAY_LOW_TRIGGER::RELAY_OFF);
          break;
      }
      break;
  }
};

void RelaySwitch::toggle() {
  this->status = !this->status;
  this->switch_relay();
};

void RelaySwitch::turn_on() {
  this->status = true;
  this->switch_relay();
};

void RelaySwitch::turn_off() {
  this->status = false;
  this->switch_relay();
};

void RelaySwitch::set_status(bool state) {
  this->status = state;
  this->switch_relay();
}

// template <typename T>
StageSwitch::StageSwitch(void) {
  ;
};

void StageSwitch::init(byte stage, byte relay_pin[], byte trigger_mode) {
  this->stage = stage;
  this->trigger_mode = trigger_mode;
  this->relay = new RelaySwitch[stage];
  for (byte i=0; i<this->stage; i++)
    this->relay[i].init(relay_pin[i], this->trigger_mode);
};

byte StageSwitch::get_status() {
  return this->status;
};

void StageSwitch::set_status(byte level) {
  if (level <= this->stage && level != this->status) {
    this->setup_complete = false;
    this->need_setup = true;
    this->status = level;
  }
};

void StageSwitch::handle() {
  if (this->need_setup) {
    for (byte i=0; i<this->stage; i++)
      this->relay[i].turn_off();
    this->setup_time = millis();  // all relay trun off time
    this->need_setup = false;
    this->setup_complete = this->status == 0 ? true : false;
  }
  else if (!this->setup_complete && (millis() - this->setup_time) >= this->delay_on_ms) {
    this->relay[(this->status - 1)].turn_on();
    this->setup_complete = true;
  }
}


