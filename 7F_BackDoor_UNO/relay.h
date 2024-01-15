#include <Arduino.h>


class RelaySwitch {
  public:
    RelaySwitch(void);
    void init(byte relay_pin, byte trigger_mode);
    bool get_status();
    void toggle();
    void turn_on();
    void turn_off();
    void set_status(bool state);
  protected:
    byte trigger_mode;
  private:
    byte pin;
    bool status = false;
    void switch_relay();
};


class StageSwitch : public RelaySwitch {
  public:
    StageSwitch(void);
    void init(byte stage, byte relay_pin[], byte trigger_mode);
    byte get_status();
    void set_status(byte level);
    void handle();
  private:
    bool need_setup = false;
    bool setup_complete = false;
    RelaySwitch * relay;
    byte stage;
    byte status = 0;
    unsigned long setup_time;
    unsigned long delay_on_ms = 1000;
};


