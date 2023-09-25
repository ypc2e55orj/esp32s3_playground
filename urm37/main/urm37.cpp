#include "urm37.hpp"

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ESP-IDF
#include <driver/gpio.h>
#include <driver/mcpwm_cap.h>
#include <rom/ets_sys.h>
#include <esp_timer.h>

// Project
#include <gpio.hpp>


class Urm37v5::Urm37v5Impl
{
private:
  // MCPWMタイマーハンドラ
  mcpwm_cap_timer_handle_t cap_timer_;
  // MCPWMチャンネルハンドラ(パルス幅測定用)
  mcpwm_cap_channel_handle_t cap_chan_;
  // URM37測定開始トリガー
  driver::Gpio trigger_;
  // シグナル時のキャプチャ数
  uint32_t sample_begin_;
  // 割り込みからデータを共有するためのキュー
  TaskHandle_t task_;

  // キャプチャー対象のエッジを検出したときに実行するコールバック関数(ISR)
  static bool callback(mcpwm_cap_channel_handle_t, const mcpwm_capture_event_data_t *event_data, void *user_data)
  {
    auto *this_ptr = reinterpret_cast<Urm37v5::Urm37v5Impl *>(user_data);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (event_data->cap_edge == MCPWM_CAP_EDGE_POS)
    {
      // 開始時のキャプチャ値を保持
      this_ptr->sample_begin_ = event_data->cap_value;
    } else
    {
      // 取得完了を通知
      xTaskNotifyFromISR(this_ptr->task_, event_data->cap_value - this_ptr->sample_begin_, eSetValueWithOverwrite,
                         &xHigherPriorityTaskWoken);
    }

    return xHigherPriorityTaskWoken == pdTRUE;
  }

  // URM37に測距開始をトリガーする
  void trigger()
  {
    trigger_.set(false);
    // 10usほどの遅延を入れないとトリガーとして認識されない
    esp_rom_delay_us(10);
    trigger_.set(true);
  }

public:
  explicit Urm37v5Impl(gpio_num_t pin_trigger, gpio_num_t pin_echo)
    : cap_timer_(nullptr), cap_chan_(nullptr),
      trigger_(pin_trigger, GPIO_MODE_OUTPUT, false, false),
      sample_begin_(0), task_(xTaskGetCurrentTaskHandle())
  {
    // タイマーを初期化
    mcpwm_capture_timer_config_t timer_config = {};
    timer_config.clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT;
    timer_config.group_id = 0;
    ESP_ERROR_CHECK(mcpwm_new_capture_timer(&timer_config, &cap_timer_));
    // チャンネルを初期化
    mcpwm_capture_channel_config_t chan_config = {};
    chan_config.gpio_num = pin_echo;
    chan_config.prescale = 1;
    chan_config.flags.neg_edge = true;
    chan_config.flags.pos_edge = true;
    chan_config.flags.pull_up = true;
    chan_config.flags.invert_cap_signal = true; // キャプチャ対象はLOW期間のため、信号を反転する
    ESP_ERROR_CHECK(mcpwm_new_capture_channel(cap_timer_, &chan_config, &cap_chan_));

    // 有効なエッジを検出した際に実行するコールバック関数
    mcpwm_capture_event_callbacks_t callbacks = {};
    callbacks.on_cap = Urm37v5::Urm37v5Impl::callback;
    ESP_ERROR_CHECK(
      mcpwm_capture_channel_register_event_callbacks(cap_chan_, &callbacks, reinterpret_cast<void *>(this)));

    // チャンネルを有効化
    ESP_ERROR_CHECK(mcpwm_capture_channel_enable(cap_chan_));
    // タイマーを有効化
    ESP_ERROR_CHECK(mcpwm_capture_timer_enable(cap_timer_));
    // タイマーを開始
    ESP_ERROR_CHECK(mcpwm_capture_timer_start(cap_timer_));
  }

  // cm精度で距離を取得する
  bool distance(float &distance_cm, uint32_t timeout_ms)
  {
    uint32_t ticks = 0;
    // トリガー
    trigger();
    // キャプチャ完了を待つ
    if (xTaskNotifyWait(0x00, ULONG_MAX, &ticks, pdMS_TO_TICKS(timeout_ms)) == pdFALSE)
    {
      return false; // タイムアウト
    }
    // キャプチャタイマーの周波数を取得
    uint32_t resolution = 0;
    esp_err_t err = mcpwm_capture_timer_get_resolution(cap_timer_, &resolution);
    if (err != ESP_OK)
    {
      return false;
    }
    // 掛かった時間[us]に換算
    float duration_us = static_cast<float>(ticks) * (1'000'000.0f / static_cast<float>(resolution));
    // mmに変換(1cmあたり50us)
    distance_cm = duration_us / 50.0f;
    return true;
  }
};

Urm37v5::Urm37v5(gpio_num_t pin_trigger, gpio_num_t pin_echo)
  : impl_(new Urm37v5Impl(pin_trigger, pin_echo))
{}

Urm37v5::~Urm37v5() = default;

bool Urm37v5::distance(float &distance_cm, uint32_t timeout_ms)
{
  return impl_->distance(distance_cm, timeout_ms);
}
