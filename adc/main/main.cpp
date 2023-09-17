#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "adc.h"
#include "average.h"

void mainTask(void *)
{
    data::MovingAverage<int, int, 512> average;
    driver::Adc adc(ADC_UNIT_1, ADC_CHANNEL_4);

    while(1)
    {
        adc.read();
        int voltage = adc.to_voltage() * 2 + 100;
        std::cout << average.update(voltage) << std::endl;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

extern "C" void app_main(void)
{
    xTaskCreatePinnedToCore(mainTask, "mainTask", 8192, NULL, 1, NULL, 1);
}
