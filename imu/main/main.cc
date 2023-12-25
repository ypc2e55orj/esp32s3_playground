// C++
#include <iostream>

// ESP-IDF
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Project
#include "spi.h"
#include "imu.h"

void mainTask(void *) {
    driver::peripherals::Spi spi(SPI2_HOST, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, 16);
    driver::hardware::Imu imu(spi, GPIO_NUM_8);

    auto xLastWakeTime = xTaskGetTickCount();
    while (true) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
        imu.update();
        auto accel = imu.linear_acceleration();
        auto gyro = imu.angular_rate();

        std::cout << "Accel X:" << accel.x << std::endl
                  << "Accel Y:" << accel.y << std::endl
                  << "Accel Z:" << accel.z << std::endl;

        std::cout << "Gyro X:" << gyro.x << std::endl
                  << "Gyro Y:" << gyro.y << std::endl
                  << "Gyro Z:" << gyro.z << std::endl;
    }
}

extern "C" void app_main(void) {
    xTaskCreatePinnedToCore(mainTask, "mainTask", 8192, nullptr, 10, nullptr, 1);
}
