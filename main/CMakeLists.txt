set(SOURCES
    main.c
    rndl_mqtt.c
    platform.c
    example/example_bitmap.c
    example/example_bitmap_data.c
    example/example_extent.c
    example/example_lines.c
    example/example_raindrops.c
    example/example_rectangle.c
    font_8x8_basic.c
)

set(REQ
    esp_event 
    esp_netif
    esp_timer
    esp_wifi
    mqtt
    nvs_flash
    rndl)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${REQ})

# Enable anonymous structs
target_compile_options(${COMPONENT_LIB} PRIVATE -fms-extensions)
# Force all warnings to be errors
target_compile_options(${COMPONENT_LIB} PRIVATE -Werror -Wall -Wextra
                        -Werror=unused-variable)
