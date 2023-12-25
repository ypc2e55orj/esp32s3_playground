file(GLOB_RECURSE SOURCES "*.cc")

idf_component_register(SRCS ${SOURCES} INCLUDE_DIRS ".")

component_compile_options(
        -std=gnu++23
        -Wall
        -Wextra
        -Wdouble-promotion
        -Wfloat-equal
        -fexceptions
)
