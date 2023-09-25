#pragma once

#include <memory>

#include <hal/gpio_types.h>

class Urm37v5
{
private:
  class Urm37v5Impl;
  std::shared_ptr<Urm37v5Impl> impl_;

public:
  explicit Urm37v5(gpio_num_t pin_trigger, gpio_num_t pin_echo);
  ~Urm37v5();

  // cm単位で距離を取得する
  bool distance(float &distance_cm, uint32_t timeout_ms);
};