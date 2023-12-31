#include "gpio.hpp"

#include <memory>

#include <driver/gpio.h>

namespace driver
{

  class Gpio::GpioImpl
  {
  private:
    gpio_num_t num_;
    bool level_;

  public:
    explicit GpioImpl(gpio_num_t num, gpio_mode_t mode, bool enable_pullup, bool enable_pulldown)
      : num_(num), level_(false)
    {
      // 引数で指定されたピンを初期化
      gpio_config_t config = {};
      config.pin_bit_mask = 1ULL << num;
      config.mode = mode;
      config.pull_up_en = enable_pullup ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
      config.pull_down_en = enable_pulldown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
      ESP_ERROR_CHECK(gpio_config(&config));
    }
    ~GpioImpl() = default;

    // 出力
    bool set(bool level)
    {
      level_ = level;
      return gpio_set_level(num_, level_ ? 1 : 0) == ESP_OK;
    }

    // 入力
    bool get()
    {
      return gpio_get_level(num_) == 1;
    }
  };

  Gpio::Gpio(gpio_num_t num, gpio_mode_t mode, bool enable_pullup, bool enable_pulldown)
    : impl_(new GpioImpl(num, mode, enable_pullup, enable_pulldown))
  {
  }
  Gpio::~Gpio() = default;

  bool Gpio::set(bool level)
  {
    return impl_->set(level);
  }

  bool Gpio::get()
  {
    return impl_->get();
  }
}