#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace z_status_led {

// Same as ESPHome's status_led (fast blink on error, slow blink on warning),
// but emits a short "heartbeat" blink pattern every interval when healthy.
class ZStatusLED : public Component {
 public:
  explicit ZStatusLED(GPIOPin *pin) : pin_(pin) {}

  void set_heartbeat_interval(uint32_t interval) { this->heartbeat_interval_ = interval; }
  void set_heartbeat_on_length(uint32_t length) { this->heartbeat_on_length_ = length; }
  void set_heartbeat_off_length(uint32_t length) { this->heartbeat_off_length_ = length; }
  void set_heartbeat_count(uint8_t count) { this->heartbeat_count_ = count; }

  // Called from the generated main() before App.setup() so the pin is driven during early boot.
  void pre_setup();
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  bool heartbeat_state_(uint32_t now) const;
  void write_(bool state);

  GPIOPin *pin_;
  uint32_t heartbeat_interval_{60000};
  uint32_t heartbeat_on_length_{100};
  uint32_t heartbeat_off_length_{100};
  uint8_t heartbeat_count_{2};
  bool state_{false};
};

}  // namespace z_status_led
}  // namespace esphome
