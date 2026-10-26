#include "z_status_led.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace z_status_led {

static const char *const TAG = "z_status_led";

void ZStatusLED::pre_setup() {
  this->pin_->setup();
  this->pin_->digital_write(false);
  this->state_ = false;
}

void ZStatusLED::dump_config() {
  ESP_LOGCONFIG(TAG, "Z Status LED:");
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Heartbeat interval: %u ms", (unsigned) this->heartbeat_interval_);
  ESP_LOGCONFIG(TAG, "  Heartbeat pattern: %u x (%u ms on, %u ms off)", (unsigned) this->heartbeat_count_,
                (unsigned) this->heartbeat_on_length_, (unsigned) this->heartbeat_off_length_);
  if (this->heartbeat_duration_ != 0)
    ESP_LOGCONFIG(TAG, "  Heartbeat duration: %u ms", (unsigned) this->heartbeat_duration_);
}

void ZStatusLED::loop() {
  const uint32_t now = millis();
  const uint32_t app_state = App.get_app_state();

  // Latched so the heartbeat doesn't come back when millis() wraps.
  if (this->heartbeat_duration_ != 0 && now >= this->heartbeat_duration_)
    this->heartbeat_expired_ = true;

  // Warnings are ignored on purpose: they fire for routine things like no API client being connected.
  if ((app_state & STATUS_LED_ERROR) != 0u) {
    this->write_(now % 250u < 150u);
  } else {
    this->write_(!this->heartbeat_expired_ && this->heartbeat_state_(now));
  }
}

bool ZStatusLED::heartbeat_state_(uint32_t now) const {
  const uint32_t period = this->heartbeat_on_length_ + this->heartbeat_off_length_;
  if (period == 0 || this->heartbeat_interval_ == 0)
    return false;
  const uint32_t t = now % this->heartbeat_interval_;
  if (t >= period * this->heartbeat_count_)
    return false;
  return t % period < this->heartbeat_on_length_;
}

void ZStatusLED::write_(bool state) {
  if (state == this->state_)
    return;
  this->state_ = state;
  this->pin_->digital_write(state);
}

float ZStatusLED::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace z_status_led
}  // namespace esphome
