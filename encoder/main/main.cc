#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "spi.h"
#include "encoder.h"

void mainTask(void *) {
    driver::peripherals::Spi spi(SPI2_HOST, GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, 2);
    driver::hardware::Encoder encoder(spi, GPIO_NUM_4);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10));
        encoder.update();
        std::cout << encoder.raw() << std::endl;
    }
}

extern "C" void app_main(void) {
    xTaskCreatePinnedToCore(mainTask, "mainTask", 8192 * 2, nullptr, 10, nullptr, 1);
}