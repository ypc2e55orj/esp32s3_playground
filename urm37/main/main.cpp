// C++
#include <iostream>

// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Project
#include "./urm37.hpp"

TaskHandle_t xMainHandle = nullptr;

constexpr auto SENSOR_TRIGGER_LEFT = GPIO_NUM_2;
constexpr auto SENSOR_ECHO_LEFT = GPIO_NUM_1;

[[noreturn]] void mainTask(void *)
{
  Urm37v5 left(SENSOR_TRIGGER_LEFT, SENSOR_ECHO_LEFT);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    float distance_cm = 0.0f;
    if (left.distance(distance_cm, 40))
    {
      std::cout << "distance: " << distance_cm << "[cm]" << std::endl;
    }
  }
}

extern "C" void app_main(void)
{
  xTaskCreatePinnedToCore(mainTask, "mainTask", 8192, nullptr, 1, &xMainHandle, 1);
}