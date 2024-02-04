set(SOURCES
    "main.c"
    "color/color.c"
    "led_driver/led_encoder.c"
    "led_driver/led_panel_driver.c"
    "platform.c"
)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ".")

# Enable anonymous structs
target_compile_options(${COMPONENT_LIB} PRIVATE -fms-extensions)
