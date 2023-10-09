#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "buzzer.hpp"

driver::hardware::Buzzer buzzer(GPIO_NUM_21);

void mainTask(void *)
{
  buzzer.set(driver::hardware::Buzzer::Mode::None, false);
  buzzer.start(8192, 10, 1);

  auto xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
  }
}

extern "C" void app_main(void)
{
  xTaskCreatePinnedToCore(mainTask, "mainTask", 8192, nullptr, 10, nullptr, 1);
}
