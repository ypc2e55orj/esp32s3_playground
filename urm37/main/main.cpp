// C++
#include <cstdio>

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_timer.h>

// Project
#include "./urm37.hpp"

TaskHandle_t xMainHandle = nullptr;
TaskHandle_t xSensorHandle = nullptr;

constexpr auto SENSOR_TRIGGER_LEFT = GPIO_NUM_2;
constexpr auto SENSOR_ECHO_LEFT = GPIO_NUM_1;
constexpr auto SENSOR_TRIGGER_RIGHT = GPIO_NUM_4;
constexpr auto SENSOR_ECHO_RIGHT = GPIO_NUM_3;


[[noreturn]] void sensorTask(void *)
{
  Urm37v5 left(SENSOR_TRIGGER_LEFT, SENSOR_ECHO_LEFT);
  Urm37v5 right(SENSOR_TRIGGER_RIGHT, SENSOR_ECHO_RIGHT);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    auto start = esp_timer_get_time();
    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
    float distance_left = 0.0f, distance_right = 0.0f;
    if (left.distance(distance_left, 100))
    {
    }
    if (right.distance(distance_right, 100))
    {
    }
    printf("[%lld]: %5.5f, %5.5f\r\n", esp_timer_get_time() - start, distance_left, distance_right);
  }
}

[[noreturn]] void mainTask(void *)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}

extern "C" void app_main(void)
{
  xTaskCreatePinnedToCore(sensorTask, "sensorTask", 8192, nullptr, 10, &xSensorHandle, 1);
  //xTaskCreatePinnedToCore(mainTask, "mainTask", 8192, nullptr, 1, &xMainHandle, 1);
}