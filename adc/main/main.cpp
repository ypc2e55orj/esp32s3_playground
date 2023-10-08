#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "battery.hpp"

driver::hardware::Battery battery(ADC_UNIT_1, ADC_CHANNEL_4);

extern "C" void app_main(void)
{
  battery.start(8192, 25, 1);
}
