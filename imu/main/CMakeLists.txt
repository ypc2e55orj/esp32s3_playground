idf_component_register(SRCS "main.cc" "imu.cc" "spi.cc"
                    INCLUDE_DIRS ".")

component_compile_options(
        -std=gnu++23
        -mtext-section-literals
        -Wall
        -Wextra
        -Wdouble-promotion
        -Wfloat-equal)

