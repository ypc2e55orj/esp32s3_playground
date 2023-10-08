idf_component_register(SRCS "main.cpp"
        "adc.cpp"
        "battery.cpp"
        INCLUDE_DIRS ".")

component_compile_options(
        -std=gnu++23
        -Wall
        -Wextra
        -Wdouble-promotion
        -Wfloat-equal
        -fexceptions
)
